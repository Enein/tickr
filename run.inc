<!DOCTYPE html>
<html lang="ru-RU">
<head>
	<meta charset="UTF-8">
	<meta name="viewport" content="width=device-width, initial-scale=1">
	<title>tickr.run</title>
	<script src="/core/jquery.min.js"></script>
</head>

<body>

<style>
body { margin: 0; }
.grid {
	display: grid;
	grid-template-rows: 1fr 1fr 1fr;
	grid-template-columns: 1fr 1fr 1fr 1fr 1fr;
	grid-gap: 2vw;
}
@media all and (min-width:1200px) {
	div.grid { grid-template-columns: 1fr 1fr 1fr 1fr 1fr 1fr; }
	div.popup { height: auto; line-height: normal; font-size: 4vw; }
}
.grid > .box {
	position: relative;
	box-sizing: border-box;
	padding: .2em 0;
	color: #fff;
	text-shadow: 0px 0 0.1em #000;
	text-align: center;
	font: 10vw 'arial';
	cursor: pointer;

	-webkit-touch-callout: none;
	-webkit-user-select: none;
	-khtml-user-select: none;
	-moz-user-select: none;
	-ms-user-select: none;
	user-select: none;
}
.clock {
	position: absolute;
	top: 0.12em;
	right: 0.15em;
	font-size: .2em;
	text-shadow: none;
	color: #fff;
	font-family: 'courier new', courier;
}
.lap {
	position: absolute;
	top: 0.15em;
	left: 0.15em;
	background-color: rgba(221, 221, 221, 0.75);
	padding: 0 0.2em;
	letter-spacing: -0.05em;
	height: 1.1em;
	line-height: 1.1em;
	color: #000;
	font-weight: bold;
	font-size: .22em;
	text-shadow: none;
}
.name {
	position: absolute;
	bottom: 0.3em;
	width: 100%;
	text-align: center;
	line-height: .9em;
	color: #fff;
	background-color: rgba(255, 255, 255, 0.2);
	font-size: .18em;
	text-shadow: 0 0 2px rgba(0, 0, 0, .5);
	overflow: hidden;
}
.pos {
	position: absolute;
	bottom: 1em;
	right: 0.14em;
	color: rgba(255, 255, 255, 0.8);
	font-size: .1em;
	         /*.2*/
	text-shadow: none;
	/*display: flex;
	align-items: center;
	height: 100%;*/
}
.box.busy { background-color: #777 !important; cursor: default; }

.params { clear: both; display: block; width: 100%; margin-top: 100px; }
.cshowbox { border-radius: 0 50% 5px 0; margin: 3px 0px; padding: 1px 5px; display: inline-block; float: left; clear: left; }
.cshowbox > input { width: 20px; height: 20px; cursor: pointer; }
.cshowbox > label { color: #fff; text-shadow: 0 0 3px #000; font: 12pt arial; cursor: pointer; }

.popup { position: fixed; width: 100%; height: 8vh; line-height: 8vh; background: rgba(186, 255, 0, 0.05); bottom: 0; text-align: center; font: 8vw Arial; opacity: 1; transition: .9s; }
.popup.magic { background: rgba(186, 255, 0, 0.4); bottom: 5%; transition: .5s; }
.popup.magic.gone { transform: translateY(200%); opacity: 0; }

.popup .popup-text { display: flex; justify-content: center; align-items: center; height: 100%; }
.popup .popup-text > div { color: #555; text-shadow: 0 0 5px #ccc; transform: scale(0.1); opacity: .2; transition: .8s; }
.popup.magic .popup-text > div { transform: scale(1); opacity: 1; }
.popup .popup-text > div b { font-size: 150%; position: relative; display: inline-block; padding: 0.12em; border: 3px solid #fff; box-shadow: inset 0px 0px 0.6em 0.1em #000; text-shadow: 0 0 7px #000; color: #fff; background-color: #fff; }
.finish-place { position: absolute; top: 0; width: 100%; text-align: center; left: 0; font-size: 0.22em; white-space: nowrap; }
.finish-name { position: absolute; bottom: 0; width: 100%; text-align: center; left: 0; font-size: 0.18em; line-height: 0.8em; }

.manage-link { display: block; position: fixed; bottom: 0; right: 0; padding: 0.2em 1em; font-family: Arial; font-size: 0.9em; background-color: #888; color: #fff; text-shadow: 0 0 1px #333; text-decoration: none; }
.manage-link:hover, .manage-link:active { background-color: #aaa; }
</style>

<div class="grid">
</div>
<div class="params"></div>
<a class="manage-link" href="/manage?rid=$RACEID$" target="_blank">К НАСТРОЙКАМ</a>

<script type="text/javascript">

var RID = $RACEID$,
	NOW = $NOW$,
	RIDERS = $RIDERS$,
	CATS = [],
	CATIDS = [],
	LIVE = $LIVE$,
	CHATID = "$CHATID$",
	CLR = [ '#009688', '#8bc34a', '#9c27b0', '#0f71bf', '#af2a00', '#ff9800', '#009688', '#8bc34a', '#9c27b0', '#0f71bf', '#af2a00', '#ff9800' ];


function speak(text) {
	const message = new SpeechSynthesisUtterance();
	message.rate = 1.25;
	message.lang = "ru-RU";
	message.text = text;
	window.speechSynthesis.speak(message);
}

function declOfNum(number, words) {  
	return words[(number % 100 > 4 && number % 100 < 20) ? 2 : [2, 0, 1, 1, 1, 2][(number % 10 < 5) ? number % 10 : 5]];
}

window.onload = function () {

	RIDERS.forEach( function (el) {
		$('.grid').append('<div id="n' + el.n + '" class="box" style="background-color: ' + CLR[el.c] + '" data-busy="no" data-maxts="' + el.ts + '" data-num="' + el.n + '" data-cat="' + el.c + '">' + el.n + '<div class="clock">00:00</div><div class="lap">' + el.l + ' / ' + el.f + '</div><div class="name">' + el.u + '</div><div class="pos">#' + el.p + '</div></div>');

		if (!CATS.includes(el.cn)) {
			CATS.push(el.cn);
			CATIDS.push(el.c);
		}
	} );

	for(var i = 0; i < CATS.length; i++) {
		$('.params').append('<div class="cshowbox" style="background-color:' + CLR[CATIDS[i]] + '"><input type="checkbox" class="cshow" id="c' + CATIDS[i] + 'show" data-catid="' + CATIDS[i] + '" checked="checked"> <label for="c' + CATIDS[i] + 'show">' + CATS[i] + '</label></div>');
	}

	$('.cshow').click(function () {
		var cid = $(this).data('catid');
		if ($(this).prop('checked')) {
			$('.box').each(function() {
				if ($(this).data('cat') == cid)
					$(this).show();
			});
		} else {
			$('.box').each(function() {
				if ($(this).data('cat') == cid)
					$(this).hide();
			});
		}

	});

	var TI = setInterval( function () {
		NOW += 400;
		$('.box').each(function() {
			var maxts = parseInt($(this).data('maxts')),
				onlap = parseInt((NOW - maxts)/1000);
			let minutes = Math.floor(onlap / 60);
			let seconds = onlap % 60;
			if (minutes < 0 || seconds < 0) {
				$(this).find('.clock').html('..:..');
			} else {
				minutes = String(minutes).padStart(2, "0");
				seconds = String(seconds).padStart(2, "0");
				$(this).find('.clock').html(minutes + ':' + seconds);
			}
		});
	}, 400);

	var TI_GLOBAL = setInterval( function () {
		$.ajax({
			type : "POST",
			url : "/sync",
			contentType: 'application/json',
			data : JSON.stringify({
				"raceid" : RID
			}),
			success : function(data) {
				var data_json = JSON.parse(data),
					n_arr = [];

				data_json.forEach( function (el) {
					var num = el.n,
						lap = el.l + ' / ' + el.f,
						ts = el.ts;
					if ($('#n' + num).length) {
						$('#n' + num).data('maxts', ts);
						$('#n' + num).find('.lap').html(lap);
					} else {
						$('.grid').append('<div id="n' + num + '" class="box" style="background-color: ' + CLR[el.c] + '" data-busy="no" data-maxts="' + ts + '" data-num="' + num + '" data-cat="' + el.c + '">' + num + '<div class="clock">00:00</div><div class="lap">' + lap + ' / ' + el.f + '</div><div class="name">' + el.u + '</div><div class="pos">#' + el.p + '</div></div>');
					}
					n_arr.push(parseInt(num));
				} );
				$('.box').each(function() {
					var n = parseInt($(this).data('num'));
					if (!n_arr.includes(n))
						$(this).remove();
				});
			}
		});
	}, 2000);

	$('body').on('dblclick', '.grid .box', function() {
		if ($(this).data('busy') != "yes") {
			var clr = $(this).css('background-color');

			$(this).data('busy', 'yes').addClass('busy');

			var num = parseInt($(this).data('num')),
				cid = parseInt($(this).data('cat')),
				name = $(this).find('.name').html(),
				box = $(this);

			$.ajax({
				type : "POST",
				url : "/lap",
				contentType: 'application/json',
				data : JSON.stringify({
					"raceid" : RID,
					"live" : LIVE,
					"chatid" : CHATID,
					"catid" : cid,
					"rnum" : num
				}),
				success : function(data) {
					var data_json = JSON.parse(data);
					if (!data_json.verified) {
						box.data('busy', 'no').removeClass('busy');
						alert('Круг меньше 15 секунд, ложная отсечка!');
					} else {
						box.data('maxts', data_json.maxts);
						box.find('.lap').html(data_json.lap);
						box.appendTo('.grid');
						if (data_json.finished != undefined) {

							var tmp_id = "f" + Math.random().toString(36).substr(2, 4);
							$('body').append('<div id="' + tmp_id + '" class="popup"><div class="popup-text"><div>НОМЕР <b id="finish-num"><div class="finish-place">МЕСТО ' + data_json.position + '</div>' + num + '<div class="finish-name">' + name + '</div></b> ФИНИШ!<div></div></div>');
							var el = document.getElementById(tmp_id);
							$('#' + tmp_id + ' b').css({ 'min-width': ($('#' + tmp_id + ' b').height() + 10) + 'px', 'border-color': clr, 'text-shadow': '0 0 7px ' + clr, 'box-shadow': 'inset 0px 0px 0.6em 0.1em ' + clr });
							setTimeout(function() {
								el.classList.add('magic');
							}, 40);
							setTimeout(function () {
								el.classList.add('gone');
								setTimeout(function () {
									el.outerHTML = "";
								}, 500);
							}, 3000);

							box.find('.clock').remove();
							var ti = setTimeout(function () { box.remove(); }, 3000);
							speak(num + '-й, ты ' + data_json.position + '-й, финиш');
						} else {
							var ti = setTimeout(function () { box.data('busy', 'no').removeClass('busy'); }, 3000);
							var lapsleft = data_json.lapsleft,
								lleft_speak = '';
							if (lapsleft > 1)
								lleft_speak = ', еще ' + lapsleft + ' ' + declOfNum(lapsleft, ['круг', 'круга', 'кругов']);
							else
								lleft_speak = ', последний круг';

							let mins = Math.floor(data_json.losing / 60);
							let secs = data_json.losing % 60;
							var losing_time = '';
							if (mins > 0)
								losing_time = mins + ' ' + declOfNum(mins, ['минута', 'минуты', 'минут']) + ' , ' + secs + ' ' + declOfNum(secs, ['секунда', 'секунды', 'секунд']);
							else
								losing_time = secs + ' ' + declOfNum(secs, ['секунда', 'секунды', 'секунд']);

							if (data_json.position < 2)
								speak(num + '-й, ты ' + data_json.position + '-й, ' + lleft_speak);
							else
								speak(num + '-й, ты ' + data_json.position + '-й, отставание ' + losing_time + ', ' + lleft_speak);
						}
					}
				}
			});
		}
	});

}
</script>

</body>

</html>