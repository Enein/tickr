<!DOCTYPE html>
<html lang="ru-RU">
<head>
	<meta charset="UTF-8">
	<meta name="viewport" content="width=device-width, initial-scale=1">
	<title>tickr.manage</title>
	<script src="/core/jquery.min.js"></script>
</head>

<body>

<style>
body { background-color: #fff; margin: 0; padding: 0; }

header { width: 100%; background-color: #607d8b; position: relative; font-family: arial; padding-bottom: 5px; }
header .h_id { position: absolute; left: 8px; top: 0; height: 20px; line-height: 20px; text-align: center; color: #ccc; }
header .h_name { width: 100%; text-align: center; padding-top: 20px; }
header .h_name input { width: 100%; color: #fff; font-size: 1em; font-weight: 700; background: transparent; border: none; text-align: center; outline: none; cursor: pointer; transition: 0.15s; }
header .h_name input.infocus { background-color: #fff; color: #607d8b; cursor: text; letter-spacing: -0.3px; }
header .h_home { position: absolute; top: 0; right: 8px; height: 20px; line-height: 20px; color: #ccc; }
header .h_home > a { color: #c6edff; border-bottom: 1px dotted #c6edff; text-decoration: none; }
header .h_home > a:hover { color: #fff; border-color: #fff; }

.newcat { width: 100%; box-sizing: border-box; margin: 0 0; padding: 6px; border: solid #ddd; border-width: 2px 0 2px 0; }
.newcat > input { border-width: 0; font-size: 12pt; line-height: 40px; color: #000; background-color: #c6edff; padding: 0 10px; }
.newcat > button { border-width: 0; font-size: 12pt; height: 40px; line-height: 40px; color: #fff; background-color: #2135a7; cursor: pointer; }
.newcat > button:hover, .newusr > button:hover, .manage > button:hover { background-color: #3951dc; }
.newcat > button:active, .newusr > button:active, .manage > button:active { background-color: #0b1a6d; }
.newcat > #newcatname { width: 40vw; }
.newcat > #newcatlaps { width: 10vw; }
.newcat > #addcat { width: 30vw; }

.newusr { width: 100%; box-sizing: border-box; margin: 0 0; padding: 6px; border: solid #ddd; border-width: 2px 0 2px 0; }
.newusr > #newusername { box-sizing: border-box; border-width: 0; font-size: 12pt; line-height: 40px; color: #000; background-color: #c6edff; padding: 0 10px; width: 100%; }
.newusr > #newusercat { border: 2px dashed #2135a7; margin-top: 10px; font-size: 12pt; height: 40px; line-height: 40px; color: #000; background-color: #ecffc6; padding: 0 10px; width: 80%; }
.newusr > #newusernum { margin-top: 10px; box-sizing: border-box; border-width: 0; font-size: 12pt; line-height: 40px; color: #000; background-color: #c6edff; padding: 0 10px; width: 17%; float: right; }
.newusr > #addusr { margin-top: 10px; display: block; border-width: 0; font-size: 12pt; height: 40px; line-height: 40px; color: #fff; background-color: #2135a7; cursor: pointer; }

.manage { width: 100%; box-sizing: border-box; margin: 0 0; padding: 6px; border: solid #ddd; border-width: 2px 0 2px 0; }
.manage > #userlist { border: 2px dashed #2135a7; font-size: 12pt; height: 40px; line-height: 40px; color: #000; background-color: #ffe2d2; padding: 0 10px; width: 100%; }
.manage > #setusercat { border: 2px dashed #2135a7; margin-top: 10px; font-size: 12pt; height: 40px; line-height: 40px; color: #000; background-color: #ecffc6; padding: 0 10px; width: 80%; }
.manage > #changecat { margin-top: 10px; display: inline-block; border-width: 0; font-size: 12pt; height: 40px; line-height: 40px; color: #fff; background-color: #2135a7; cursor: pointer; }
.manage > #dnfuser { margin-top: 10px; display: inline-block; border-width: 0; font-size: 12pt; height: 40px; line-height: 40px; color: #fff; background-color: #2135a7; cursor: pointer; }
.manage > #deluser { margin-top: 10px; display: block; border-width: 0; font-size: 10pt; height: 20px; line-height: 20px; color: #fff; background-color: #2135a7; cursor: pointer; }

.cat-wrapper { margin-top: 10px; width: 100%; overflow-x: scroll; overflow-y: scroll; height: 370px; }
.cat {
   display: grid;
   grid-template-columns: repeat($CAT_CNT$, 1fr);
   grid-template-rows: 50px 1fr;
   min-width: 900px;
}
.category:nth-child(odd) { background: #cffae2; }
.category:nth-child(even) { background: #e8f2ff; }
.category > h3 { width: 100%; text-align: center; padding: 8px 0 4px; margin: 0; border-bottom: 1px dashed #b1b1b1; font: 1em Arial; font-weight: 700; color: #444; position: relative; white-space: nowrap; }
h3 > .cid { position: absolute; left: 0; font-size: 0.8em; background: #909ad5; color: #fff; padding: 0 3px; top: 0; }
.catlist { position: relative; font-size: 8pt; background: inherit; padding-top: 8px; }

.usr { display: block; position: relative; height: 40px; }
.unumber { float: left; width: 20%; min-width: 30px; height: 26px; padding-top: 4px; background: #86eaff; text-align: center; font: 14pt courier; font-weight: 700; }
.unumber.notset { background-color: #ff723f; cursor: pointer; }
.realname { float: left; width: 75%; }
.realname > input { width: 100%; border: none; border-bottom: 1px solid #ddd; font-family: Arial; font-size: 10pt; padding: 3px 0px 1px 4px; }
.nick { text-align: left; font: 8pt Arial; color: #673ab7; font-weight: 700; padding-left: 4px; }


.ttl { margin: 20px 0 0; padding: 5px 20px; font-family: sans-serif; font-size: 11pt; color: #5e658a; }

.start-wrapper { margin-top: 25px; }
.start-wrapper fieldset { position: relative; }

#fixmaxlaps { position: absolute; top: -36px; right: 0; cursor: pointer; opacity: 0.5; width: 20px; height: 20px; }

.options-wrapper { width: 100%; margin: 10px 0; background-color: #c6edff; font: 12pt Arial; height: 30px; line-height: 30px; position: relative; }
.options-wrapper #overlap { width: 24px; height: 24px; cursor: pointer; }
.options-wrapper label { display: inline-block; position: absolute; top: 2px; left: 36px; cursor: pointer; letter-spacing: -0.5px; white-space: nowrap; }

.cs { width: 100%; }
.cs > input.catready { width: 30px; height: 30px; }
.cs > label { font: 12pt arial; }
.cs > .claps { height: 30px; line-height: 30px; box-sizing: border-box; border-width: 0; font-size: 14pt; color: #000; background-color: #89ffbe; padding: 0 0 0 10px; min-width: 50px; }
.cs > .claps:disabled { background-color: #efdada; }

.tlg-wrapper { width: 100%; margin: 10px 0; background-color: #c6edff; font: 12pt Arial; height: 30px; line-height: 30px; position: relative; }
.tlg-wrapper #tlg { width: 24px; height: 24px; cursor: pointer; }
.tlg-wrapper label { display: inline-block; position: absolute; top: 2px; left: 36px; cursor: pointer; }
.tlg-wrapper #tlg-chat { float: right; height: 30px; width: 32%; font-size: 1.1em; font-weight: 700; background-color: #ecffc6; }

#res { text-decoration: none }
#res:hover button { background-color: #267fa9; font-weight: 700; }
#start:hover { background-color: #15bc1b; font-weight: 700; }

#start { display: block; border-width: 0; font-size: 12pt; height: 40px; line-height: 40px; color: #fff; background-color: #3ea242; cursor: pointer; width: 100%; }
#start:disabled { cursor: not-allowed; background-color: #999; }
#results { display: block; border-width: 0; font-size: 12pt; height: 40px; line-height: 40px; color: #fff; background-color: #607D8B; cursor: pointer; width: 100%; }

.ctime { display: none; }

</style>

<div class="main">

<header><div class="h_id"></div><div class="h_name"><input id="racetitle" type="text" value=""/></div><div class="h_home"><a href="/">выйти</a></div></header>

<p class="ttl">Добавить новую категорию?</p>
<div class="newcat">
<input id="newcatname" type="text" placeholder="Название категории" value=""> <input id="newcatlaps" type="text" placeholder="Кругов" value="" title="Кругов"> <button id="addcat">ДОБАВИТЬ</button>
</div>

<p class="ttl">Добавить нового участника?</p>
<div class="newusr">
<input id="newusername" type="text" placeholder="Фамилия Имя" value="">
<select id="newusercat">$CAT_HTML_LIST$</select><input id="newusernum" type="text" placeholder="№" value="" title="Номерок">
<button id="addusr">ДОБАВИТЬ</button>
</div>

<p class="ttl">Управление участниками</p>
<div class="manage">
<select id="userlist"></select>
<select id="setusercat">$CAT_HTML_LIST$</select>
<button id="changecat">СМЕНИТЬ КАТЕГОРИЮ</button> <button id="dnfuser">ОФОРМИТЬ СХОД</button>
<button id="deluser">УДАЛИТЬ ИЗ ПРОТОКОЛА</button>
</div>

<div class="cat-wrapper">
	<p class="ttl">Таблица участников</p>
	<div class="cat">
		$CAT_HTML$
	</div>
</div>

<div class="start-wrapper">

	<div class="options-wrapper">
		<input type="checkbox" id="overlap" checked="checked"> <label for="overlap">Финиш округленным после финиша лидера</label>
	</div>

	<fieldset>
		<legend><p class="ttl">Дать старт категориям</p></legend>
		$CAT_START$
		<input id="fixmaxlaps" type="checkbox"/>
	</fieldset>

	<div class="tlg-wrapper">
		<input type="checkbox" id="tlg"> <label for="tlg">Telegram Bot live stream</label> <select id="tlg-chat"><option value="@tickrlive">@tickrlive</option><option value="@maryanofflive">@maryanofflive</option></select>
	</div>

	<p><button id="start">НАЧАТЬ ХРОНОМЕТРАЖ</button></p>
	<p><a id="res" href="/results?rid=" target="_blank"><button id="results">РЕЗУЛЬТАТЫ</button></a></p>

</div>

<a id="run" href="/run" target="_blank">.</a>

</div>

<script type="text/javascript">

var race = $DATA_RACE$,
	categories = $DATA_CATS$,
	users = $DATA_USERS$;

var run_link = '/run?rid=' + race.id + "&l=" + race.live + "&t=" + race.chatid;

window.onload = function () {

$('#run').attr('href', run_link);
$('#res').attr('href', '/results?rid=' + race.id + "&live=" + race.live + "&chatid=" + race.chatid);

$('.h_id').html('#' + race.id + ' / <span style="color:' + ((race.online) ? '#bfff00;">online' : '#e08974;">offline') + '</span>');
$('#racetitle').val(race.name);

$('#overlap').prop('checked', (race.overlap) ? 'checked' : '');

if (race.live)
	$("#tlg").prop('checked', 'checked');
if (race.chatid != null)
	$("#tlg-chat").val(race.chatid);

users.forEach( function(u) {
	$('#c' + u.catid + 'u').append('<div class="usr"><div class="unumber' + ((u.number >= 0) ? '' : ' notset') + '" data-number="' + u.number + '" data-name="' + u.realname + '" data-cat="' + categories[u.catid] + '" data-uid="' + u.uid + '">' + ((u.number >= 0) ? u.number : '?') + '</div><div class="realname"><input class="realname" type="text" value="' + u.realname + '" readonly="readonly" onfocus="select(this);"><div class="nick">' + u.nickname + '</div></div>');
	$('#userlist').append('<option value="' + u.number + '" data-cat="' + u.catid + '" data-uid="' + u.uid + '">(' + ((u.number >= 0) ? u.number : '?') + ') ' + u.realname + '</option>');
});

$('#userlist').change(function() {
	$('#setusercat').val($(this).find('option:selected').data('cat'));
});

$('#addcat').click(function(){
	if ($('#newcatname').val() && $('#newcatlaps').val()) {
		$.ajax({
			type : "POST",
			url : "/addcat",
			contentType: 'application/json',
			data : JSON.stringify({
				"raceid" : race.id,
				"cname" : $('#newcatname').val(),
				"claps" : $('#newcatlaps').val()
			}),
			success : function(data) {
				var data_json = JSON.parse(data);
				if (data_json.catid > 0)
					location.reload();
			}
		});
	}
});

$('#addusr').click(function(){
	if ($('#newusername').val() && $('#newusernum').val()) {
		$.ajax({
			type : "POST",
			url : "/addusr",
			contentType: 'application/json',
			data : JSON.stringify({
				"raceid" : race.id,
				"uname" : $('#newusername').val(),
				"ucat" : $('#newusercat option:selected').val(),
				"unum" : $('#newusernum').val()
			}),
			success : function(data) {
				var data_json = JSON.parse(data);
				if (data_json.number > 0)
					location.reload();
				else {
					users.forEach( function(u) {
						if (u.number == parseInt($('#newusernum').val()))
							alert('Участник #' + u.number + ' (' + u.realname + ') уже зарегистрирован! Выберите другой номер...');
					});
				}
			}
		});
	} else
		alert('Нужно заполнить ФИО и номер участника.');
});

$('#changecat').click(function(){
	if (window.confirm("Точно сменить категорию участнику " + $('#userlist option:selected').text() + " на " + $('#setusercat option:selected').text() + "?")) {
		$.ajax({
			type : "POST",
			url : "/changecat",
			contentType: 'application/json',
			data : JSON.stringify({
				"raceid" : race.id,
				"uid" : $('#userlist option:selected').data('uid'),
				"ucat" : parseInt($('#setusercat option:selected').val())
			}),
			success : function(data) {
				var data_json = JSON.parse(data);
				if (data_json.uid >= 0)
					location.reload();
			}
		});
	}
});

$('#dnfuser').click(function(){
	if (window.confirm("Точно оформить СХОД участнику " + $('#userlist option:selected').text() + "?")) {
		$.ajax({
			type : "POST",
			url : "/dnfuser",
			contentType: 'application/json',
			data : JSON.stringify({
				"raceid" : race.id,
				"uid" : $('#userlist option:selected').data('uid')
			}),
			success : function(data) {
				var data_json = JSON.parse(data);
				if (data_json.uid >= 0)
					alert('Номер ' + $('#userlist').val() + ' удален из заезда');
				else if (data_json.uid == -2)
					alert('Номер ' + $('#userlist').val() + ' не участвует в текущем заезде!');
			}
		});
	}
});

$('#deluser').click(function(){
	if (window.confirm("Точно удалить участника " + $('#userlist option:selected').text() + "?")) {
		$.ajax({
			type : "POST",
			url : "/deluser",
			contentType: 'application/json',
			data : JSON.stringify({
				"raceid" : race.id,
				"uid" : $('#userlist option:selected').data('uid')
			}),
			success : function(data) {
				location.reload();
			}
		});
	}
});

$('.claps').bind("change paste keyup", function(){
	var v_laps = $(this).val(),
		v_cat = $(this).data("catid");
	if (v_laps.length > 0 && parseInt(v_laps) > 0 && parseInt(v_laps) < 1000) {
		$.ajax({
			type : "POST",
			url : "/changelaps",
			contentType: 'application/json',
			data : JSON.stringify({
				"raceid" : race.id,
				"catid" : v_cat,
				"laps" : parseInt(v_laps)
			}),
			success : function(data) {
				console.log(data);
			}
		});
	}
});

$('#start').click(function(){
	var cats = {};

	$(this).prop('disabled', true);

	$('.catready').each(function() {
		if ($(this).prop('checked'))
			cats[$(this).data('catid')] = $('#c' + $(this).data('catid') + 'laps').val();
	});
	$.ajax({
		type : "POST",
		url : "/start",
		contentType: 'application/json',
		data : JSON.stringify({
			"raceid" : race.id,
			"live" : race.live,
			"chatid" : race.chatid,
			"catstartlist" : cats
		}),
		success : function(data) {
			//document.getElementById('run').click();
			window.open(run_link, '_blank');
			setTimeout(function () {
				location.reload();
			}, 1000);
		}
	});
});

$('#fixmaxlaps').click(function(){
	if ($(this).prop('checked')) {
		$('.claps').each(function() {
			if ($(this).prop('disabled')) {
				$(this).addClass('unlock');
				$(this).prop('disabled', '');
			}
		});
		alert('РАЗРЕШЕНО изменение количества кругов стартовавшим категориям');
	} else {
		$('.claps.unlock').each(function() {
			$(this).removeClass('unlock');
			$(this).prop('disabled', 'disabled');
		});
	}
});

$('#tlg').click(function(){
	var LIVE = ($(this).prop('checked')),
		CHATID = $('#tlg-chat').val();

	$.ajax({
		type : "POST",
		url : "/tlgliveon",
		contentType: 'application/json',
		data : JSON.stringify({
			"raceid" : race.id,
			"live" : LIVE,
			"chatid" : CHATID
		}),
		success : function(data) {
			var data_json = JSON.parse(data);
			if (data_json.raceid > 0) {
				if (LIVE)
					alert('Трансляция гонки в Telegram (' + $('#tlg-chat').val() + ') ВКЛЮЧЕНА!');
				else
					alert('Трансляция гонки в Telegram (' + $('#tlg-chat').val() + ') ОТКЛЮЧЕНА!');
				location.reload();
			} else
				alert('Что-то пошло не так.');
		}
	});
});

$('#tlg-chat').change(function(){
	var LIVE = ($('#tlg').prop('checked')),
		CHATID = $(this).val();

	$.ajax({
		type : "POST",
		url : "/tlgliveon",
		contentType: 'application/json',
		data : JSON.stringify({
			"raceid" : race.id,
			"live" : LIVE,
			"chatid" : CHATID
		}),
		success : function(data) {
			var data_json = JSON.parse(data);
			if (data_json.raceid > 0) {
			} else
				alert('Что-то пошло не так.');
		}
	});
});

$('#overlap').click(function(){
	var OVERLAP = $(this).prop('checked');

	$.ajax({
		type : "POST",
		url : "/options_overlap",
		contentType: 'application/json',
		data : JSON.stringify({
			"raceid" : race.id,
			"overlap" : OVERLAP
		}),
		success : function(data) {
			var data_json = JSON.parse(data);
			console.log(data_json);
		}
	});
});

$('#racetitle').focus(function () {
	$(this).addClass('infocus');
}).focusout(function () {
	$(this).removeClass('infocus');
	if ($(this).val().length > 3 && $(this).val() != race.name) {
		let v_name = $(this).val();
		$.ajax({
			type : "POST",
			url : "/rename_race",
			contentType: 'application/json',
			data : JSON.stringify({
				"raceid" : race.id,
				"name" : v_name
			}),
			success : function(data) {
				var data_json = JSON.parse(data);
				console.log(data_json);
				race.name = v_name;
			}
		});
	}
});

$('.unumber').click(function () {
	var v_uid = $(this).data('uid'),
		v_name = $(this).data('name'),
		v_cat = $(this).data('cat'),
		v_unum = $(this);
	if (v_unum.hasClass('notset')) {
		var inp = prompt("Номер для " + v_name + " (категория " + v_cat + ")");
		if (inp.length > 0) {
			if (!isNaN(inp)) {
				var v_num = parseInt(inp);
				if (v_num >= 0) {
					$.ajax({
						type : "POST",
						url : "/set_user_number",
						contentType: 'application/json',
						data : JSON.stringify({
							"raceid" : race.id,
							"uid" : v_uid,
							"unum" : v_num
						}),
						success : function(data) {
							var data_json = JSON.parse(data);
							if (data_json.number >= 0) {
								v_unum.removeClass('notset')
									.html(data_json.number)
									.data('number', data_json.number);
								$('#userlist option[data-uid="' + v_uid + '"]').val(data_json.number)
									.html('(' + data_json.number +  ') ' + v_name);
								users.forEach(function(u) {
									if (u.uid == v_uid)
										u.number = data_json.number;
								});
							} else {
								users.forEach(function(u) {
									if (u.number == v_num)
										alert('Участник #' + u.number + ' (' + u.realname + ') уже зарегистрирован! Выберите другой номер...');
								});
							}
						}
					});
				}
			}
		}
	}
});

$('.unumber').dblclick(function() {
	var v_unum = $(this),
		v_uid = v_unum.data('uid'),
		v_name = v_unum.data('name'),
		v_num = v_unum.data('number');
	if (!v_unum.hasClass('notset')) {
		if (window.confirm("Отобрать номер " + v_num + " у участника " + v_name + "?")) {
			$.ajax({
				type : "POST",
				url : "/set_user_number",
				contentType: 'application/json',
				data : JSON.stringify({
					"raceid" : race.id,
					"uid" : v_uid,
					"unum" : -99
				}),
				success : function(data) {
					var data_json = JSON.parse(data);
					if (data_json.number == -1) {
						v_unum.addClass('notset')
							.html('?')
							.data('number', data_json.number);
						$('#userlist option[data-uid="' + v_uid + '"]').val(data_json.number)
							.html('(?) ' + v_name);
						users.forEach(function(u) {
							if (u.uid == v_uid)
								u.number = data_json.number;
						});
					} else {
						alert('Не удалось (вероятно, участник уже стартовал)');
					}
				}
			});
		}
	}
});

}

</script>

</body>

</html>