var config = require('./config.json');

const fs = require('fs'),
  path = require('path'),
  rimraf = require('rimraf'),
  {exec} = require('child_process'),
  express = require('express'),
  bodyParser = require('body-parser'),
  groupBy = require('json-groupby');
  iconv = require('iconv-lite'),
  multer = require('multer'),
  net = require('net'),
  request = require('request'),
  { Pool } = require('pg');

var application_root = __dirname;
var content;

const setup = { port: config.port }
const app = express();
      app.use(bodyParser.json());
const pgconnect = {
  host: 'localhost',
  port: config.db_port,
  database: config.db_name,
  max: 20,
  user: config.db_user,
  password: config.db_passwd
};

const TLG_BOT = config.telegram_bot;


RegExp.escape = function(text) {
  return text.replace(/[-[\]{}()*+?.,\\^$|#\s]/g, "\\$&");
}

String.prototype.mapReplace = function (replacements) {
  Object.keys(replacements).forEach(function(key) {
    var val = replacements[key];
    delete replacements[key];
    key = '$' + key + '$';
    replacements[key] = val;
  });
  var regex = [];
  for (var prop in replacements) {
    regex.push(RegExp.escape(prop));
  }
  regex = new RegExp( regex.join('|'), "g" );
  return this.replace(regex, function(match){
    return replacements[match];
  });
}

readFileSync_encoding = function (filename, encoding) {
  var content = fs.readFileSync(filename);
  return iconvlite.decode(content, encoding);
}

do_tlg_post = function (json_data, tlg_method) {
  request({
    url: 'https://api.telegram.org/' + TLG_BOT + '/' + tlg_method,
    method: 'POST',
    json: true,
    body: json_data
  }, function (error, response, body){
  	//console.log(error, response, body); 
  });
}


const db = new Pool(pgconnect);
db.on('error', function (err, client) {
  console.log('idle client error! err.message -> ' + err.message + ', err.stack -> ' + err.stack);
  process.exit(1);
});

app.use( express.static( application_root ) );

app.get('/', (req, res) => {
  var content = fs.readFileSync('index.inc', 'utf8');
  let sql = "select id, name, online, created, protocol from races order by id desc";
  db.query(sql, function(err, out) {
    if (err) {
      console.log('error -> ' + err, out, sql);
      res.send( content );
    } else {
      var data = out.rows;
      content = content.mapReplace({
        'EXCNT' : data.length,
        'EXLIST' : JSON.stringify(data)
      });
      res.send( content );
    }
  });
});

app.get('/race', (req, res) => {
  if (req.query.do == 'new') {
    var content = fs.readFileSync('newrace.inc', 'utf8');
    res.send( content );
  } else {
    var rid = parseInt(req.query.raceid),
        v_loc = "";
    if (rid > 0) {
      v_loc = "/manage?rid=" + rid;
    } else
      v_loc = "/";
    res.send( '<html><head></head><body><script type="text/javascript">window.location.href = "' + v_loc + '";</script></body></html>' );
  }
});


var storage = multer.diskStorage({
  destination: function (req, file, cb) {
    let dir = 'data/' + file.originalname.replace(/[\.\(\)-]/g, '_') + '_' + parseInt(Date.now()/1000) + '/';
    if ( !fs.existsSync(dir) )
      fs.mkdirSync(dir);
    cb(null, dir)
  },
  filename: function (req, file, cb) {
    cb(null, file.originalname)
  }
});

var upload = multer( { storage: storage } ).single( 'file' );

app.post('/protocol', (req, res) => {
  upload(req, res, (err) => {
    var FN = req.file.filename,
      FP = req.file.path;
    if (err) {
      console.log('/protocol -> function failed, error -> ' + err);
      return res.status( 200 ).send( '0' );
    }
    return res.status( 200 ).send( '{ "filename" : "' + FN + '", "filepath" : "' + FP + '" }' );
  });
});

app.post('/createrace', (req, res) => {
  var data = req.body;

  let sql = 'INSERT INTO RACES (NAME, PROTOCOL, ONLINE, CREATED, OVERLAP) VALUES ($1, $2, $3, $4, $5) RETURNING id',
  params = [
    data.title,
    data.filepath,
    false,
    new Date(),
    true
  ];
  db.query(sql, params, function(err, out) {
    if (err)
      console.log(err);

    var RID = out.rows[0].id;
    res.send( '{ "rid" : ' + RID + '}' );

    if (data.filename.length) {
      var csv = fs.readFileSync(application_root + '/' + data.filepath),
        csv_nice = iconv.encode(iconv.decode(csv, 'cp1251'), 'utf8').toString();

      var csv_arr = csv_nice.split(/\r?\n\r?\n/);
      csv_arr.shift();
      csv_arr.forEach(function (el, ix) {
        var c = el.split(/\r?\n/);
        var c0 = c.shift().replace(';;;;;;;', '');

        let sql = 'INSERT INTO CATEGORIES (CATID, CATNAME, RACEID, MAXLAPS, ONLINE) VALUES ($1, $2, $3, $4, $5) RETURNING catid',
        params = [
          ix + 1,
          c0,
          RID,
          1,
          false
        ];

        let catlist = c;

        db.query(sql, params, function(err, out) {
          if (err)
            console.log(err);
          else {
            var CID = out.rows[0].catid;

            catlist.forEach(function (u) {
              if (u.length > 0) {
                var v = u.split(';');

                if (!v[0])
                  v[0] = -1;

                let sql = 'INSERT INTO USERS (NUMBER, REALNAME, NICKNAME, CATID, RACEID, LAPSDONE, FINISHED, STARTED) VALUES ($1, $2, $3, $4, $5, $6, $7, $8)',
                params = [
                  v[0],
                  v[1],
                  v[2],
                  CID,
                  RID,
                  0,
                  false,
                  false
                ];
                db.query(sql, params, function(err, out) {
                  if (err)
                    console.log(err);
                });

              }
            });
          }

        });
      });
    }

  });
});


app.get('/manage', (req, res) => {
  var raceid = ((req.query.rid) ? parseInt(req.query.rid) : 0);

  if (raceid > 0) {
    var content = fs.readFileSync('manage.inc', 'utf8');

    let sql = "select id, name, online, live, chatid, overlap from races where id = " + raceid;
    db.query(sql, function(err, out) {
      if (err) {
        console.log('error -> ' + err, out, sql);
        res.send( content );
      } else {
        var data = out.rows[0];
        content = content.mapReplace({
          'DATA_RACE' : JSON.stringify(data)
        });
        let sql = "select catid, catname, maxlaps, online from categories where raceid = " + raceid + " order by catid asc";
        db.query(sql, function(err, out) {
          if (err) {
            console.log('error -> ' + err, out, sql);
            res.send( content );
          } else {
            var data = out.rows,
              html_out = '',
              html_out_list = '',
              html_out_cat_start_list = '',
              cats_array = [];
            data.forEach( function(item) {
              html_out += '<div class="category"><h3 id="c' + item.catid + '">' + item.catname  + '<div class="cid">#' + item.catid + '</div></h3><div class="catlist" id="c' + item.catid + 'u"></div></div>';
              html_out_list += '<option value="' + item.catid + '">' + item.catname  + '</option>';
              var tmp = (item.online) ? ' disabled="disabled"' : '';
              var tmp2 = (item.online) ? ' style="text-decoration: line-through"' : '';
              html_out_cat_start_list += '<div class="cs"><input type="checkbox" class="catready" id="c' + item.catid + 'start" data-catid="' + item.catid + '"' + tmp + '> <label for="c' + item.catid + 'start"' + tmp2 + '>' + item.catname + '</label> <input class="claps" type="number" id="c' + item.catid + 'laps" min="1" max="100" value="' + item.maxlaps + '"' + tmp + ' data-catid="' + item.catid + '"> <span style="color:green;font-weight:700;">кругов</span> или <input class="claps ctime" type="number" id="c' + item.catid + 'time" min="1" max="1440" value=""' + tmp + ' data-catid="' + item.catid + '"> <span style="color:orange;text-decoration:line-through;">минут</span></div>';
              
              cats_array[item.catid] = item.catname;
            });
            content = content.mapReplace({
              'CAT_CNT' : (data.length > 0 ? data.length : 1),
              'CAT_HTML' : html_out,
              'CAT_HTML_LIST' : html_out_list,
              'CAT_START' : html_out_cat_start_list,
              'DATA_CATS' : JSON.stringify(cats_array)
            });

            let sql = "select uid, number, realname, nickname, catid from users where raceid = " + raceid + " order by catid, number asc";
            db.query(sql, function(err, out) {
              if (err) {
                console.log('error -> ' + err, out, sql);
                res.send( content );
              } else {
                var data = out.rows;
                content = content.mapReplace({
                  'DATA_USERS' : JSON.stringify(data)
                });
                res.send( content );
              }
            });
          }
        });
      }
    });
  } else
    res.send( '' );
});



app.post('/addcat', (req, res) => {
  var data = req.body;

  let sql = "select max(catid) from categories where raceid = " + data.raceid;
  db.query(sql, function(err, out) {
    if (err) {
      console.log('error -> ' + err, out, sql);
      res.send( '' );
    } else {
      var ddata = parseInt(out.rows[0].max);
      if (isNaN(ddata))
        ddata = 0;

      let sql = 'INSERT INTO CATEGORIES (CATID, CATNAME, RACEID, MAXLAPS, ONLINE) VALUES ($1, $2, $3, $4, $5) RETURNING catid',
      params = [
        ddata + 1,
        data.cname,
        data.raceid,
        data.claps,
        false
      ];
      db.query(sql, params, function(err, out) {
        if (err)
          console.log(err);
        var CID = out.rows[0].catid;
        res.send( '{ "catid" : ' + CID + '}' );

      });

    }
  });

});

app.post('/addusr', (req, res) => {
  var data = req.body;

  if (data.unum) {
    let sql = "select count(*) from users where number = " + data.unum + " and raceid = " + data.raceid;
    db.query(sql, function(err, out) {
      if (err) {
        console.log('error -> ' + err, out, sql);
        res.send( '' );
      } else {
        var cnt = parseInt(out.rows[0].count);

        if (cnt > 0)
          res.send( '{ "number" : -1 }' );
        else {
          let sql = 'INSERT INTO USERS (NUMBER, REALNAME, NICKNAME, CATID, RACEID, LAPSDONE, FINISHED, STARTED) VALUES ($1, $2, $3, $4, $5, $6, $7, $8) RETURNING number',
          params = [
            data.unum,
            data.uname,
            '',
            data.ucat,
            data.raceid,
            0,
            false,
            false
          ];
          db.query(sql, params, function(err, out) {
            if (err)
              console.log(err);

          	console.log('/addusr', data);

            var UID = out.rows[0].number;
            res.send( '{ "number" : ' + UID + '}' );

          });
        }
      }
    });
  } else
    res.send( 'number is not set' );

});

app.post('/changecat', (req, res) => {
  var data = req.body;
  console.log('/changecat', data);

  if (data.raceid && data.uid && data.ucat) {
    let sql = 'UPDATE USERS SET (CATID, LAPSDONE, FINISHED, STARTED) = ($1, $2, $3, $4) WHERE raceid = ' + data.raceid + ' and uid = ' + data.uid + ' RETURNING uid, number',
    params = [
      data.ucat,
      0,
      false,
      false
    ];
    db.query(sql, params, function(err, out) {
      if (err)
        console.log(err);
      else if (out.rows.length > 0)
        res.send( '{ "uid" : ' + out.rows[0].uid + ', "number" : ' + out.rows[0].number + '}' );
      else
        res.send( '{ "uid" : -2, "number" : -1 }' );
    });
  } else
    res.send( '{ "uid" : -1, "number" : -1 }' );
});

app.post('/dnfuser', (req, res) => {
  var data = req.body;
  console.log('/dnfuser', data);

  if (data.raceid && data.uid) {
    let sql = 'UPDATE USERS SET FINISHED = true WHERE raceid = ' + data.raceid + ' and uid = ' + data.uid + ' and started = true and finished = false RETURNING uid, number';
    db.query(sql, function(err, out) {
      if (err)
        console.log(err);
      else if (out.rows.length > 0)
        res.send( '{ "uid" : ' + out.rows[0].uid + ', "number" : ' + out.rows[0].number + '}' );
      else
        res.send( '{ "uid" : -2, "number" : -1 }' );
    });
  } else
    res.send( '{ "uid" : -1, "number" : -1 }' );
});

app.post('/deluser', (req, res) => {
  var data = req.body;
  console.log('/deluser', data);

  if (data.raceid && data.uid) {
    let sql = 'DELETE FROM USERS WHERE raceid = ' + data.raceid + ' and uid = ' + data.uid;
    db.query(sql, function(err, out) {
      if (err)
        console.log(err);
      else
        res.send( 'del ok' );

    });
  } else
    res.send( 'uid is not set' );
});

app.post('/changelaps', (req, res) => {
  var data = req.body;
  console.log('/changelaps', data);

  if (data.laps) {
    let sql = 'UPDATE CATEGORIES SET MAXLAPS = $1 WHERE raceid = ' + data.raceid + ' and catid = ' + data.catid + ' RETURNING catid',
    params = [
      data.laps
    ];
    db.query(sql, params, function(err, out) {
      if (err)
        console.log(err);
      var CID = out.rows[0].catid;
      res.send( '{ "catid" : ' + CID + '}' );

    });
  } else
    res.send( 'maxlaps is not set' );
});

app.post('/tlgliveon', (req, res) => {
  var data = req.body;
  console.log('/tlgliveon', data);

  if (data.chatid) {
    let sql = 'UPDATE RACES SET (LIVE, CHATID) = ($1, $2) WHERE id = ' + data.raceid + ' RETURNING id',
    params = [
      data.live,
      data.chatid
    ];
    db.query(sql, params, function(err, out) {
      if (err)
        console.log(err);
      var ID = out.rows[0].id;
      res.send( '{ "raceid" : ' + ID + '}' );

    });
  } else
    res.send( '{ "raceid" : 0 }' );
});

app.post('/options_overlap', (req, res) => {
  var data = req.body;
  console.log('/options_overlap', data);

  let sql = 'UPDATE RACES SET OVERLAP = ' + data.overlap + ' WHERE id = ' + data.raceid + ' RETURNING id';
  db.query(sql, function(err, out) {
    if (err)
      console.log(err);
    var ID = out.rows[0].id;
    res.send( '{ "raceid" : ' + ID + '}' );

  });
});

app.post('/rename_race', (req, res) => {
  var data = req.body;
  console.log('/rename_race', data);

  let sql = 'UPDATE RACES SET NAME = \'' + data.name + '\' WHERE id = ' + data.raceid + ' RETURNING id';
  db.query(sql, function(err, out) {
    if (err)
      console.log(err);
    var ID = out.rows[0].id;
    res.send( '{ "raceid" : ' + ID + '}' );

  });
});

app.post('/set_user_number', (req, res) => {
  var data = req.body;
  console.log('/set_user_number', data);

  if (data.raceid && data.uid && (data.unum >= 0 || data.unum == -99)) {
    let sql = "select count(*) from users where number = " + data.unum + " and raceid = " + data.raceid;
    db.query(sql, function(err, out) {
      if (err) {
        console.log('error -> ' + err, out, sql);
        res.send( '' );
      } else {
        var cnt = parseInt(out.rows[0].count);
        if (data.unum == -99) {
          cnt = 0;
          data.unum = -1;
        }
        if (cnt > 0)
          res.send( '{ "number" : -1 }' );
        else {
          let sql = 'UPDATE USERS SET NUMBER = ' + data.unum + ' WHERE raceid = ' + data.raceid + ' and uid = ' + data.uid + ' and started = false and finished = false RETURNING number';
          db.query(sql, function(err, out) {
            if (err)
              console.log(err);
            if (out.rows.length > 0) {
              var UID = out.rows[0].number;
              res.send( '{ "number" : ' + UID + '}' );
            } else
              res.send( '{ "number" : -2 }' );

          });
        }
      }
    });
  } else
    res.send( 'number is not set' );

});


app.post('/start', (req, res) => {
  var data = req.body;

  var RID = data.raceid,
    cat_obj = data.catstartlist;

  console.log('/start', data);

  var v_cats = '';
  for (var k in cat_obj){
    if (cat_obj.hasOwnProperty(k)) {
      v_cats += k + ',';

      db.query('UPDATE RACES SET ONLINE = true WHERE online = false and id = ' + RID + ' RETURNING name', function(err, out) {
        if (err)
          console.log(err);
        else {
          if (out.rowCount > 0) {
            var RNAME = out.rows[0].name;
            var t_data = { 'chat_id' : data.chatid, 'parse_mode' : 'HTML', 'text' : '🚵 Велоперегони <b>' + RNAME + '</b> розпочато! 🚵' };
            if (data.live)
              do_tlg_post(t_data, "sendMessage");
          }
        }
      });

      let sql = 'UPDATE CATEGORIES SET (MAXLAPS, ONLINE) = ($1, $2) WHERE raceid = ' + RID + ' and online = false and catid = ' + k + ' RETURNING catid, catname, maxlaps',
      params = [
        cat_obj[k],
        true
      ];
      db.query(sql, params, function(err, out) {
        if (err)
          console.log(err);
        else {
          if (out.rowCount > 0) {

            let sql = "select count(*) from users where catid = " + out.rows[0].catid + " and number >= 0 and raceid = " + RID;
            db.query(sql, function(err, v_out) {
              if (err) {
                console.log('error -> ' + err, v_out, sql);
              } else {
                var t_data = { 'chat_id' : data.chatid, 'parse_mode' : 'HTML', 'text' : '📢 Стартує категорiя <b>' + out.rows[0].catname + '</b>!\nКiл: <b>' + out.rows[0].maxlaps + '</b>\nУчасникiв: <b>' + v_out.rows[0].count + '</b>\nОп-оп-оп!' };
                if (data.live)
                  setTimeout(function () {
                    do_tlg_post(t_data, "sendMessage");
                  }, 500);
              }
            });
          }
        }
      });

    }
  }

  if (v_cats.length > 1) {
    v_cats = v_cats.slice(0, -1);
    let sql = 'SELECT number, catid FROM USERS WHERE raceid = ' + RID + ' and catid in (' + v_cats + ') and finished = false and started = false and number >= 0';
    db.query(sql, function(err, out) {
      if (err)
        console.log(err);
      else {

        let sql = 'UPDATE USERS SET started = true WHERE raceid = ' + RID + ' and catid in (' + v_cats + ') and started = false and number >= 0';
        db.query(sql, function(err, out2) {
          if (err)
            console.log(err);
          else {

            out.rows.forEach( function (el) {
              let sql = 'INSERT INTO LAPS (RACEID, CATID, LAPNUM, RNUMBER, TIME, NICETIME, TS, POSITION) VALUES ($1, $2, $3, $4, $5, $6, $7, $8) RETURNING lapid',
              params = [
                RID,
                el.catid,
                1,
                el.number,
                0,
                '',
                new Date().getTime(),
                0
              ];
              db.query(sql, params, function(err, out) {
                if (err)
                  console.log(err);
              });
            });

          }
        });
      }
    });
  }

  res.send( 'ok' );

});


app.get('/run', (req, res) => {
  var RID = parseInt(req.query.rid),
      RLIVE = req.query.l,
      RCHATID = req.query.t;
  if (RID > 0) {

    var content = fs.readFileSync('run.inc', 'utf8');

    let sql = 'select number N, a.catid C, b.catname CN, a.realname U, b.maxlaps F, lapsdone + 1 L, max(ts) TS, a.position P from users a inner join categories b on a.catid = b.catid inner join laps c on a.number = c.rnumber where a.raceid = ' + RID + ' and b.raceid = ' + RID + ' and b.online = true and a.finished = false and a.started = true and number >= 0 group by number, a.catid, b.catname, a.realname, b.maxlaps, lapsdone, a.position order by ts, number desc;';
    db.query(sql, function(err, out) {
      if (err) {
        console.log(err);
        res.send( '' );
      } else {
        var NOW = new Date().getTime();

        content = content.mapReplace({
          'RACEID' : RID,
          'LIVE' : RLIVE,
          'CHATID' : RCHATID,
          'NOW' : NOW,
          'RIDERS' : JSON.stringify(out.rows)
        });

        res.send( content );
      }
    });

  } else
    res.send( '' );
});

app.post('/sync', (req, res) => {
  var data = req.body;
  var RID = parseInt(data.raceid);
  if (RID > 0) {
    let sql = 'select number N, a.catid C, a.realname U, b.maxlaps F, lapsdone + 1 L, max(ts) TS, a.position P from users a inner join categories b on a.catid = b.catid inner join laps c on a.number = c.rnumber where a.raceid = ' + RID + ' and b.raceid = ' + RID + ' and b.online = true and a.finished = false and a.started = true and number >= 0 group by number, a.catid, a.realname, b.maxlaps, lapsdone, a.position order by ts, number desc;';
    db.query(sql, function(err, out) {
      if (err)
        console.log(err);
      res.send( JSON.stringify(out.rows));
    });
  } else
    res.send( '' );
});


var toHHMMSS = (secs) => {
  var sec_num = parseInt(secs, 10)
  var hours   = Math.floor(sec_num / 3600)
  var minutes = Math.floor(sec_num / 60) % 60
  var seconds = sec_num % 60

  return [hours,minutes,seconds]
      .map(v => v < 10 ? "0" + v : v)
      .filter((v,i) => v !== "00" || i > 0)
      .join(":")
}

app.post('/lap', (req, res) => {
  var data = req.body;

  // data.raceid  data.rnum  data.catid      data.live  data.chatid

  var now_time = new Date().getTime();
  let sql = "select max(ts) ts, max(lapnum) lap from laps where rnumber = " + data.rnum + " and raceid = " + data.raceid + " and catid = " + data.catid;
  db.query(sql, function(err, out) {
    if (err)
      console.log('error -> ' + err, out, sql);

    var lap = parseInt(out.rows[0].lap),
      prev_time = parseInt(out.rows[0].ts),
      diff_msec = now_time - prev_time,
      diff_sec = parseInt(diff_msec/1000);

    if (diff_sec < 15) {
      res.send( '{ "verified" : false }');
      return false;
    }

    let mins = Math.floor(diff_sec / 60);
    let secs = diff_sec % 60;
    mins = String(mins).padStart(2, "0");
    secs = String(secs).padStart(2, "0");
    let nicetime = mins + ':' + secs;

    let a_sql = `select max(position) pos from laps where raceid = ${data.raceid} and catid = ${data.catid} and lapnum = ${lap}`;
    db.query(a_sql, function(err, out) {
      if (err) {
        console.log('error -> ' + err, out, a_sql);
        res.send('');
      } else {
        let v_pos = 1 + parseInt(out.rows[0].pos);

        let b_sql = `UPDATE USERS SET (LAPSDONE, POSITION) = ((SELECT LAPSDONE + 1 FROM USERS WHERE raceid = ${data.raceid} and number = ${data.rnum}), ${v_pos}) WHERE raceid = ${data.raceid} and number = ${data.rnum} RETURNING lapsdone`;
        db.query(b_sql, function(err, out) {
          if (err) {
            console.log('error -> ' + err, out, b_sql);
            res.send('');
          } else {
            let v_lapsdone = out.rows[0].lapsdone;

            let c_sql = `UPDATE LAPS SET (TIME, NICETIME, POSITION) = ($1, $2, $3) where raceid = ${data.raceid} and rnumber = ${data.rnum} and lapnum = ${lap}`,
            params = [
              diff_msec,
              nicetime,
              v_pos
            ];
            db.query(c_sql, params, function(err, out) {
              if (err) {
                console.log('error -> ' + err, out, c_sql);
                res.send('');
              } else {
                let d_sql = `select max(lapsdone) rslt from users where catid = ${data.catid} and raceid = ${data.raceid} and finished = true 
                              union all
                           select maxlaps from categories where catid = ${data.catid} and raceid = ${data.raceid} 
                              union all 
                           select count(*) from users where catid = ${data.catid} and raceid = ${data.raceid} 
                              union all 
                           select sum(time) from laps where catid = ${data.catid} and raceid = ${data.raceid} and rnumber = ${data.rnum} 
                              union all 
                           select overlap::int from races where id = ${data.raceid}
                              union all
                           select ts from (select ts from laps where catid = ${data.catid} and raceid = ${data.raceid} and rnumber != ${data.rnum} and lapnum = ${lap} and position < ${v_pos} order by ts desc limit 1) foo`;
                db.query(d_sql, function(err, out) {
                  if (err) {
                    console.log('error -> ' + err, out, d_sql);
                    res.send('');
                  } else {
                    var cat_maxlaps = parseInt(out.rows[1].rslt),
                      usrs_finished = parseInt(out.rows[0].rslt) - cat_maxlaps,
                      cat_userscount = out.rows[2].rslt,
                      v_totaltime = parseInt(out.rows[3].rslt),
                      opt_overlap_finish = (parseInt(out.rows[4].rslt) > 0),
                      losing_time = 0;

                    var losing_text = '';
                    if (out.rows[5] != undefined)
                      losing_time = Math.round((now_time - parseInt(out.rows[5].rslt))/1000);

                    if (losing_time > 0 && losing_time < 60 && v_pos > 1)
                      losing_text = ' (+' + losing_time + ')';
                    if ((usrs_finished >= 0 || v_lapsdone >= cat_maxlaps) && v_pos > 1)
                      losing_text = ' (+' + toHHMMSS(losing_time) + ')';

                    let e_sql = "select catname str from categories where catid = " + data.catid + " and raceid = " + data.raceid + " union all select realname from users where catid = " + data.catid + " and raceid = " + data.raceid + " and number = " + data.rnum;
                    db.query(e_sql, function(err, out) {
                      if (err) {
                        console.log('error -> ' + err, out, e_sql);
                      } else
                        if (data.live) {
                          var t_data = { 
                            'chat_id' : data.chatid, 
                            'parse_mode' : 'HTML', 
                            'text' : `(${data.rnum}) <b>${out.rows[1].str}</b> <i>(${out.rows[0].str})</i>\n<pre>Час кола [${v_lapsdone}/${cat_maxlaps}] - ${nicetime}${losing_text}\nПозицiя - ${v_pos}/${cat_userscount}</pre>`, 
                            'disable_notification' : true 
                          };
                          if ((usrs_finished >= 0 && opt_overlap_finish) || v_lapsdone >= cat_maxlaps) {
                            if (v_pos == 1)
                              t_data.text += '\n<b>Переможець!</b> 🥇';
                            else if (v_pos == 2)
                              t_data.text += '\n<b>Срiбло!</b> 🥈';
                            else if (v_pos == 3)
                              t_data.text += '\n<b>Бронза!</b> 🥉';
                            else
                              t_data.text += '\n<b>Фiнiш!</b>';
                            
                            t_data.text += '\n<pre>Загальний час - ' + toHHMMSS(v_totaltime/1000) + '.' + (v_totaltime % 1000) + '</pre>';
                          }
                          do_tlg_post(t_data, "sendMessage");
                        }

                    });

                    if ((usrs_finished >= 0 && opt_overlap_finish) || v_lapsdone >= cat_maxlaps) {

                      let v_sql = `UPDATE USERS SET finished = true WHERE raceid = ${data.raceid} and number = ${data.rnum} RETURNING number`;
                      db.query(v_sql, function(err, out) {
                        if (err) {
                          console.log('error -> ' + err, out, v_sql);
                          res.send('');
                        } else {
                          var RNUM = parseInt(out.rows[0].number);
                          res.send( '{ "verified" : true, "finished" : ' + RNUM + ', "position" : ' + v_pos + ' }' );
                        }
                      });

                    } else {

                      let v_sql = 'INSERT INTO LAPS (RACEID, CATID, LAPNUM, RNUMBER, TIME, NICETIME, TS, POSITION) VALUES ($1, $2, $3, $4, $5, $6, $7, $8) RETURNING lapnum',
                      params = [
                        data.raceid,
                        data.catid,
                        lap + 1,
                        data.rnum,
                        0,
                        '',
                        now_time,
                        0
                      ];
                      db.query(v_sql, params, function(err, out) {
                        if (err) {
                          console.log('error -> ' + err, out, v_sql);
                          res.send('');
                        } else
                          res.send( '{ "verified" : true, "lap" : ' + out.rows[0].lapnum + ', "maxts" : ' + now_time + ', "lapsleft" : ' + (cat_maxlaps - v_lapsdone) + ', "position" : ' + v_pos + ', "losing" : ' + losing_time + ' }' );
                      });
                    }
                  }
                });
              }
            });
          }
        });
      }
    });

    //let v_sql = 'UPDATE USERS SET LAPSDONE = (SELECT LAPSDONE + 1 FROM USERS WHERE raceid = ' + data.raceid + ' and number = ' + data.rnum + ') WHERE raceid = ' + data.raceid + ' and number = ' + data.rnum + ' RETURNING lapsdone';
    // db.query(v_sql, function(err, out) {
    //   if (err)
    //     console.log(err);
    //   var LD = out.rows[0].lapsdone;

    //   let sql = 'UPDATE LAPS SET (TIME, NICETIME) = ($1, $2) where raceid = ' + data.raceid + ' and rnumber = ' + data.rnum + ' and lapnum = ' + lap,
    //   params = [
    //     diff_msec,
    //     nicetime,
    //   ];
    //   db.query(sql, params, function(err, out) {
    //     if (err)
    //       console.log(err);

    //     let sql = `select max(lapsdone) rslt from users where catid = ${data.catid} and raceid = ${data.raceid} and finished = true 
    //                   union all
    //                select maxlaps from categories where catid = ${data.catid} and raceid = ${data.raceid} 
    //                   union all 
    //                select count(*) from users where catid = ${data.catid} and raceid = ${data.raceid} and lapsdone >= ${LD} 
    //                   union all 
    //                select count(*) from users where catid = ${data.catid} and raceid = ${data.raceid} 
    //                   union all 
    //                select sum(time) from laps where catid = ${data.catid} and raceid = ${data.raceid} and rnumber = ${data.rnum} 
    //                   union all 
    //                select overlap::int from races where id = ${data.raceid}
    //                   union all
    //                select ts from (select ts from laps where catid = ${data.catid} and raceid = ${data.raceid} and rnumber != ${data.rnum} and lapnum >= ${LD} order by ts desc limit 1) foo`;
    //     db.query(sql, function(err, out) {
    //       if (err) {
    //         console.log('error -> ' + err, out, sql);
    //         res.send( '' );
    //       } else {
    //         var max_laps_in_cat = parseInt(out.rows[1].rslt),
    //           finished_cnt = parseInt(out.rows[0].rslt) - max_laps_in_cat
    //           position = parseInt(out.rows[2].rslt),
    //           losing = 0,
    //           cnt_in_cat = out.rows[3].rslt,
    //           total_time = parseInt(out.rows[4].rslt),
    //           overlap_finish = (parseInt(out.rows[5].rslt) > 0);

    //         var losing_text = '';
    //         if (out.rows[6] != undefined)
    //           losing = Math.round((now_time - parseInt(out.rows[6].rslt))/1000);

    //         if (losing > 0 && losing < 100 && position > 1)
    //           losing_text = ' (+' + losing + ')';
    //         if ((finished_cnt >= 0 || LD >= max_laps_in_cat) && position > 1)
    //           losing_text = ' (+' + toHHMMSS(losing) + ')';

    //         let t_sql = "select catname str from categories where catid = " + data.catid + " and raceid = " + data.raceid + " union all select realname from users where catid = " + data.catid + " and raceid = " + data.raceid + " and number = " + data.rnum;
    //         db.query(t_sql, function(err, out) {
    //           if (err)
    //             console.log('error -> ' + err, out, t_sql);

    //           if (data.live) {
    //             var t_data = { 'chat_id' : data.chatid, 'parse_mode' : 'HTML', 'text' : '(' + data.rnum + ') <b>' + out.rows[1].str + '</b> <i>(' + out.rows[0].str + ')</i>\n<pre>Час кола [' + LD + '/' + max_laps_in_cat + '] - ' + nicetime + losing_text + '\nПозицiя - ' + position + '/' + cnt_in_cat + '</pre>', 'disable_notification' : true };
    //             if ((finished_cnt >= 0 && overlap_finish) || LD >= max_laps_in_cat) {
    //               if (position == 1)
    //                 t_data.text += '\n<b>Переможець!</b> 🥇';
    //               else if (position == 2)
    //                 t_data.text += '\n<b>Срiбло!</b> 🥈';
    //               else if (position == 3)
    //                 t_data.text += '\n<b>Бронза!</b> 🥉';
    //               else
    //                 t_data.text += '\n<b>Фiнiш!</b>';
                  
    //               t_data.text += '\n<pre>Загальний час - ' + toHHMMSS(total_time/1000) + '.' + (total_time % 1000) + '</pre>';
    //             }
    //             do_tlg_post(t_data, "sendMessage");
    //           }

    //         });

    //         if ((finished_cnt >= 0 && overlap_finish) || LD >= max_laps_in_cat) {

    //           let v_sql = 'UPDATE USERS SET finished = true WHERE raceid = ' + data.raceid + ' and number = ' + data.rnum + ' RETURNING number';
    //           db.query(v_sql, function(err, out) {
    //             if (err)
    //               console.log(err);
    //             var RNUM = parseInt(out.rows[0].number);
    //             res.send( '{ "verified" : true, "finished" : ' + RNUM + ', "position" : ' + position + ' }' );
    //           });

    //         } else {

    //           let sql = 'INSERT INTO LAPS (RACEID, CATID, LAPNUM, RNUMBER, TIME, NICETIME, TS) VALUES ($1, $2, $3, $4, $5, $6, $7) RETURNING lapnum',
    //           params = [
    //             data.raceid,
    //             data.catid,
    //             lap + 1,
    //             data.rnum,
    //             0,
    //             '',
    //             now_time
    //           ];
    //           db.query(sql, params, function(err, out) {
    //             if (err)
    //               console.log(err);
    //             res.send( '{ "verified" : true, "lap" : ' + out.rows[0].lapnum + ', "maxts" : ' + now_time + ', "lapsleft" : ' + (max_laps_in_cat - LD) + ', "position" : ' + position + ', "losing" : ' + losing + ' }' );
    //           });

    //         }
    //       }
    //     });

    //   });
    // });
  });

});


app.get('/results', (req, res) => {
  var RID = parseInt(req.query.rid),
      RLIVE = req.query.live,
      RCHATID = req.query.chatid;
  if (RID > 0) {

    var content = fs.readFileSync('results.inc', 'utf8'),
        tmp_content = "";

    let a_sql = 'select ((select count(*) from users where raceid = ' + RID + ' and number >= 0) - (select count(*) from users where raceid = ' + RID + ' and finished = true and number >= 0)) isdone';
    db.query(a_sql, function(err, a_out) {
      if (err) {
        console.log(err);
      } else {
        var race_is_done = parseInt(a_out.rows[0].isdone);
        if (race_is_done == 0)
          db.query('UPDATE RACES SET ONLINE = false WHERE online = true and id = ' + RID, function(err, out) {
            if (err)
              console.log(err);
          });
      }
    });

    let s_sql = 'select lapnum, rnumber, nicetime from laps where raceid = ' + RID + ' order by lapnum asc';
    db.query(s_sql, function(err, l_out) {
      if (err) {
        console.log(err);
        res.send( '' );
      } else {

        let sql = 'select number, realname, nickname, lapsdone, c.catname, sum(time) from users a left join laps b on a.number = b.rnumber left join categories c on a.catid = c.catid where a.raceid = ' + RID + ' and b.raceid = ' + RID + ' and a.catid = b.catid and a.raceid = c.raceid and finished = true group by number, realname, nickname, lapsdone, c.catname order by catname, lapsdone desc, sum asc;';
        db.query(sql, function(err, out) {
          if (err) {
            console.log(err);
            res.send( '' );
          } else {

            var v_cat = '',
              v_pos = 0;
            out.rows.forEach( function(u) {
              if (v_cat != u.catname) {
                v_cat = u.catname;
                v_pos = 0;
                tmp_content += '<tr class="hr"><td colspan="7"><hr></td></tr>';
              }
              v_pos++;
              tmp_content += '<tr class="user">';
              v_bkg = 'transparent';
              if (v_pos == 1)
                v_bkg = '#ffe717';
              else if (v_pos == 2)
                v_bkg = '#d2d2d2';
              else if (v_pos == 3)
                v_bkg = '#e86d16';
              tmp_content += '<td style="background-color:' + v_bkg + '">' + v_pos + '</td>';
              tmp_content += '<td>' + u.number + '</td>';
              tmp_content += '<td>' + u.catname + '</td>';
              namesurname = u.realname.split(' ');
              tmp_content += '<td>' + u.realname + '</td>';
              // tmp_content += '<td>' + namesurname[0] + '</td>';
              // if (namesurname.length > 1)
              //   tmp_content += '<td>' + namesurname[1] + '</td>';
              // else
              //   tmp_content += '<td> </td>';
              tmp_content += '<td>' + u.nickname + '</td>';
              tmp_content += '<td>' + u.lapsdone + '</td>';
              tmp_content += '<td>' + toHHMMSS(Math.floor(u.sum/1000)) + '</td>';

              for(var i = 0; i < l_out.rows.length; i++) {
                if (l_out.rows[i].rnumber == u.number) {
                  tmp_content += '<td style="text-align:left">';
                  tmp_content += l_out.rows[i].nicetime + ' ';
                  tmp_content += '</td>';
                }
              }

              tmp_content += '</tr>';
            });

            content = content.mapReplace({
              'TABLE' : tmp_content
            });

            res.send( content );
          }
        });
      }
    });

  } else
    res.send( '' );
});


const Pageres = require('pageres');

app.get('/print', (req, res) => {
  var v_d = new Date();
  var v_dstr = "-" +
    v_d.getFullYear() + "-" +
    ("00" + (v_d.getMonth() + 1)).slice(-2) + "-" +
    ("00" + v_d.getDate()).slice(-2) + "-" +
    ("00" + v_d.getHours()).slice(-2) + "-" +
    ("00" + v_d.getMinutes()).slice(-2) + "-" +
    ("00" + v_d.getSeconds()).slice(-2);

  (async () => {
    await new Pageres({delay: 1})
      .src(req.query.link, ['1100x300'], { crop: false, filename: 'data/res' + v_dstr, css: '#prnt { display: none }'})
      .dest(__dirname)
      .run(function(){});
    setTimeout(function(){
      if (req.query.live == "true") {

        var r = request.post('https://api.telegram.org/' + TLG_BOT + '/sendDocument?disable_notification=true&caption=' + encodeURIComponent('Попереднi результати') + '&chat_id=' + req.query.chatid, function (err, resp, body) {
          if (err)
            console.log(err);
        });
        var form = r.form();
        form.append('document', fs.createReadStream('data/res' + v_dstr + '.png'));
      }

      res.status(200).send('<img src="/data/res' + v_dstr + '.png"/>');
    }, 500);
  })();
});


const SRV = app.listen(setup.port, () => {
  console.log('blaster.server started on port %s', setup.port);
});
SRV.timeout = 300000;
