<!DOCTYPE html>
<html lang="ru-RU">
<head>
	<meta charset="UTF-8">
	<meta name="viewport" content="width=device-width, initial-scale=1">
	<title>tickr.newrace</title>
	<script src="/core/jquery.min.js"></script>
</head>

<body>

<style>
a, select, input, button { font-size: 1.3em; }
body { min-height: auto; overflow-y: hidden; background-color: #fff; }
div.main { }
</style>

<div class="main">
	<p>
		<input type="text" id="name" placeholder="Название гонки/события">
	</p>
	<p>Протокол Bikeportal (CSV) <sup style="color:#aaa">необязательно</sup>:</p>
	<p>
		<input type="file" id="protocol">
	</p>
	<p>
		<button id="create" disabled>Создать</button>
	</p>
</div>

<script type="text/javascript">

window.onload = function () {

$('#name').keyup(function(){
	if ($(this).val().length > 3) {
		$('#create').prop('disabled', '');
	} else {
		$('#create').prop('disabled', 'disabled');
	}
});

var createRace = function (title, filename, filepath) {
	$.ajax({
		type : "POST",
		url : "/createrace",
		contentType: 'application/json',
		data : JSON.stringify({
			"title" : title,
			"filename" : filename,
			"filepath" : filepath
		}),
		success : function(data) {
			var data_json = JSON.parse(data);
			setTimeout(function () {
				window.location.href = '/manage?rid=' + data_json.rid;
			}, 2000);
		}
	});
}

$('#create').click(function(){
	var formdata = new FormData();
	if($('#protocol').prop('files').length > 0) {
		var file =$('#protocol').prop('files')[0];
		formdata.append("file", file);

		$.ajax({
			type : "POST",
			url : "/protocol",
			contentType: 'multipart/form-data',
			data : formdata,
			processData: false,
			contentType: false,
			success : function(data) {
				if (data == '0') {
					console.log('error on protocol uploading');
				} else {
					var json_data = JSON.parse(data);
					createRace($('#name').val(), json_data.filename, json_data.filepath);
				}
			}
		});
	} else {
		createRace($('#name').val(), '', '');
	}

	$(this).prop('disabled', 'disabled');
});

}
</script>

</body>

</html>