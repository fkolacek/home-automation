<?php
  $remote = "esp-tvremote.example.com";
  $vendor = "SAMSUNG";
  $codes = Array(
    'program_up' => '707048B7',
    'power' => '707030CF',
    'up' => '707042BD',
    'ok' => '7070629D',
    'program_down' => '7070C837',
    'left' => '707022DD',
    'down' => '7070C23D',
    'right' => '7070A25D',
    'menu' => '7070CD32',
    'volume_up' => '707028D7',
    'volume_down' => '7070A857',
    'volume_mute' => '7070B04F',
    '1' => '7070807F',
    '2' => '707040BF',
    '3' => '7070C03F',
    '4' => '707020DF',
    '5' => '7070A05F',
    '6' => '7070609F',
    '7' => '7070E01F',
    '8' => '707010EF',
    '9' => '7070906F',
    '0' => '707000FF',
    'return' => '7070AD52',
    'input' => '7070F00F'
  );

  if(isset($_GET['code']) && !empty($_GET['code'])){
    $code = $_GET['code'];

    if(array_key_exists($code, $codes)){
      //$output = file_get_contents(sprintf("http://%s/?vendor=%s&code=%s", $remote, $vendor, $codes[$code]));
      $c = curl_init();
      curl_setopt($c, CURLOPT_URL, sprintf("http://%s/?vendor=%s&code=%s", $remote, $vendor, $codes[$code]));
      curl_setopt($c, CURLOPT_RETURNTRANSFER, 1);
      $output = curl_exec($c);
      curl_close($c);

      if(isset($_GET['api'])){
        header('Content-type: application/json');
        die($output);
      }
    }
  }
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
  "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
<head>
  <title>TV Remote</title>
  <meta http-equiv="content-type" content="text/html;charset=utf-8" />
  <style type="text/css">
  *{ border: 0; padding: 0; margin: 0; }
  body{ background: #303030 url('bg.png') no-repeat; }
  .button{ display: block; width: 88px; height: 88px; position: absolute; }  
  </style>
</head>
<body>
  <a href="?code=program_up" id="program_up" class="button" style="top: 19px; left: 56px;"></a>
  <a href="?code=power" id="power" class="button" style="top: 19px; left: 155px;"></a>
  <a href="?code=up" id="up" class="button" style="top: 19px; left: 254px;"></a>
  <a href="?code=ok" id="ok" class="button" style="top: 19px; left: 352px;"></a>
  <a href="?code=program_down" id="program_down" class="button" style="top: 118px; left: 56px;"></a>
  <a href="?code=left" id="left" class="button" style="top: 118px; left: 155px;"></a>
  <a href="?code=down" id="down" class="button" style="top: 118px; left: 254px;"></a>
  <a href="?code=right" id="right" class="button" style="top: 118px; left: 352px;"></a>
  <a href="?code=menu" id="menu" class="button" style="top: 216px; left: 56px;"></a>
  <a href="?code=volume_up" id="volume_up" class="button" style="top: 216px; left: 155px;"></a>
  <a href="?code=volume_down" id="volume_down" class="button" style="top: 216px; left: 254px;"></a>
  <a href="?code=volume_mute" id="volume_mute" class="button" style="top: 216px; left: 352px;"></a>
  <a href="?code=1" id="1" class="button" style="top: 314px; left: 56px;"></a>
  <a href="?code=2" id="2" class="button" style="top: 314px; left: 155px;"></a>
  <a href="?code=3" id="3" class="button" style="top: 314px; left: 254px;"></a>
  <a href="?code=4" id="4" class="button" style="top: 314px; left: 352px;"></a>
  <a href="?code=5" id="5" class="button" style="top: 412px; left: 56px;"></a>
  <a href="?code=6" id="6" class="button" style="top: 412px; left: 155px;"></a>
  <a href="?code=7" id="7" class="button" style="top: 412px; left: 254px;"></a>
  <a href="?code=8" id="8" class="button" style="top: 412px; left: 352px;"></a>
  <a href="?code=9" id="9" class="button" style="top: 510px; left: 56px;"></a>
  <a href="?code=0" id="0" class="button" style="top: 510px; left: 155px;"></a>
  <a href="?code=return" id="return" class="button" style="top: 510px; left: 254px;"></a>
  <a href="?code=input" id="input" class="button" style="top: 510px; left: 352px;"></a>

  <script type="text/javascript" src="jquery.js"></script>
  <script type="text/javascript">
  $(document).ready(function(){
    $(".button").click(function(){
      var c = $(this).attr("id");

      $.getJSON("index.php", { api: 1, code: c }, function(data){
      });

      return false;
    });
  });
  </script>
</body>
</html>

