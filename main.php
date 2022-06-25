<?php

 define('HOST','localhost');
 define('USER','u454075123_HID');
 define('PASS','QAZwerty!23');
 define('DB','u454075123_user');

 //membuat koneksi dengan database
 $con = mysqli_connect(HOST,USER,PASS,DB) or die('Unable to Connect');
 global $con;
 //  if($con){
 //         echo "koneksi host berhasil.<br/>";
 // }else{
 //         echo "koneksi gagal.<br/>";
 // }
 ?>

 <?php
 		$query=mysqli_query($con, "SELECT * FROM elektro");

 		if (mysqli_num_rows($query) > 0) {
 			# buat array
 			$responsistem = array();
 			$responsistem["data"] = array();
 			$i=1;
 			while ($row = mysqli_fetch_assoc($query)) {
 				# kerangka format penampilan data json
 				$data["state".$i] =  $row['state'];
 				$i++;
 			}
 		}
 			echo json_encode($data);
 ?>
