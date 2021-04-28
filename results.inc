<!DOCTYPE html>
<html lang="ru-RU">
<head>
	<meta charset="UTF-8">
	<meta name="viewport" content="width=device-width, initial-scale=1">
	<title>tickr.results</title>
	<script src="/core/jquery.min.js"></script>
</head>

<body>

<style>
table { font:10pt courier; text-align: center; border-collapse: collapse; }
tr.user:nth-child(odd) { background-color: #ecffc6; }
tr.user:nth-child(even) { background-color: #ffe2d2; }
th, td { border: 1px solid #777; }
tr.hr, tr.hr td { border-width: 0; }
th { font-weight: 700; }
th, td { padding: 2px 5px; font-family: courier; font-size: 10pt; }
</style>

<table>
	<thead>
		<tr>
			<th>Результат</th>
			<th>Номер</th>
			<th>Категория</th>
			<th>ФИО</th>
			<!--<th>Фамилия</th>-->
			<!--<th>Имя</th>-->
			<th>Ник (bp)</th>
			<th>Кругов</th>
			<th>Общее время</th>
		</tr>
	</thead>
	<tbody>
		$TABLE$
	</tbody>
</table>

<p><a id="prnt" href="/print?link=">печать</a></p>

<script>
document.getElementById("prnt").href += location.href;
</script>

</body>
</html>