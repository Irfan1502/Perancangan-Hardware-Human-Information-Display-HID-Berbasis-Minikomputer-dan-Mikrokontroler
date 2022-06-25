<!DOCTYPE html>
<html>
<head>
	<title>Human Information Display</title>
	<link rel="stylesheet" href="https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/css/bootstrap.min.css" integrity="sha384-ggOyR0iXCbMQv3Xipma34MD+dH/1fQ784/j6cY/iJTQUOhcWr7x9JvoRxT2MZw1T" crossorigin="anonymous">
	<link rel="stylesheet" type="text/css" href="web_styleab.css">
</head>
<body>
<div class="container-fluid">
			<div class="col-10 offset-2" style="margin-top: 20px;">
				<div class="row">
					<div class="mx-5" id="9" style="transform: rotate(90deg);"></div>
					<div class="mx-5" id="10" style="transform: rotate(90deg);"></div>
					<div class="mx-5" id="11" style="transform: rotate(90deg);"></div>
					<div class="mx-5" id="12" style="transform: rotate(90deg);"></div>
					<div class="mx-5" id="13" style="transform: rotate(90deg);"></div>
					<div class="mt-3">Ruang Prodi Elektro</div>
					<div class="mt-4">HID</div>
				</div>
			</div>
			<div class="row">
				<div class="col-3" style="margin-top: 50px;">
					<div class="mt-5" id="8" style="margin-left: 20px"></div>
					<div class="mt-5" id="7" style="margin-left: 20px"></div>
					<div class="mt-5" id="6" style="margin-left: 20px"></div>
				</div>
				<div class="col-3 offset-6">
					<div class="" id="14" style="transform: rotate(180deg); margin-top: 400px; margin-right: 250px"></div>
					<div class="" id="15" style="transform: rotate(90deg); margin-left: -35px; margin-top: 150px"></div>
				</div>
			</div>
		<div class="col-8 offset-2" style="margin-top: -90px;">
				<div class="row">
					<div class="mx-5" id="5" style="transform: rotate(-90deg);"></div>
					<div class="mx-5" id="4" style="transform: rotate(-90deg);"></div>
					<div class="mx-5" id="3" style="transform: rotate(-90deg);"></div>
					<div class="mx-5" id="2" style="transform: rotate(-90deg);"></div>
					<div class="mx-5" id="1" style="transform: rotate(-90deg);"></div>
					<div class="mt-2">Pintu Ruangan</div>
			</div>
			</div>
	</div>
<script type="text/javascript">
	function liveUpdate(){

		setInterval(function(){
			fetch('web_koneksi.php').then(function(response){
				return response.json();
			}).then(function(data){
				<?php for ($i = 1; $i <=15; $i++) :?>
					if (data.state<?=$i?> == 0) {
						document.getElementById(<?=$i?>).innerHTML = '<div id="box-merah"><p class="text">'+<?=$i;?>+'</p></div>';
					}else if (data.state<?=$i?> == 1) {
						document.getElementById(<?=$i?>).innerHTML = '<div id="box-hijau"><p class="text">'+<?=$i;?>+'</p></div>';
					}else if (data.state<?=$i?> == 2) {
						document.getElementById(<?=$i?>).innerHTML = '<div id="box-kuning"><p class="text">'+<?=$i;?>+'</p></div>';
					}
				<?php endfor;?>
			}).catch(function(error){
				console.log(error);
			});
		},2000);
	}
	document.addEventListener('DOMContentLoaded', function(){
        liveUpdate();
      });
</script>

<div id="current_date"></p>
<script>
	date = new Date();
	year = date.getFullYear();
	month = date.getMonth() + 1;
	day = date.getDate();
	document.getElementById("current_date").innerHTML = day + " / " + month + " / " + year;
</script>

<div id = "digital-clock"> </div>
	<script src = "script1.js"> </script>

<div class="pengumuman">
			<img src="qr.png">
</div>

</body>
</html>
