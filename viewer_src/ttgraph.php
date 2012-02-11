<?php

$db_user = '';
$db_pass = '';
$db_host = 'localhost';
$db_name = '';

$db_conn = mysql_pconnect($db_host, $db_user, $db_pass) or die('01');
mysql_select_db($db_name, $db_conn) or die('02');

define('COL_WIDTH', 16);

if (@$_GET['m'] != ""){
    $month = $_GET['m'];
} else {
    die('Month Error');
}

if (@$_GET['y'] != ""){
    $year = $_GET['y'];
} else {
    die('Year Error');
}

if (@$_GET['if'] != ""){
    $iface = $_GET['if'];
} else {
    die('IFace Error');
}

if (@$_GET['id'] == ""){
    die('User ID Error');
}


function weekday($month, $day, $year){
    $ix = 0;
    $tx = 0;
    $vx = 0;

	switch ($month) {
		case 2 :
		case 6 :
			$vx = 0;
			break;
		case 8 :
			$vx = 4;
			break;
		case 10 :
			$vx = 8;
			break;
		case 9 :
		case 12 :
			$vx = 12;
			break;
		case 3 :
		case 11 :
			$vx = 16;
			break;
		case 1 :
		case 5 :
			$vx = 20;
			break;
		case 4 :
		case 7 :
			$vx = 24;
			break;
	}
	if ($year > 1900) // 1900 was not a leap year
		$year -= 1900;
	$ix = (($year - 21) % 28) + $vx + ($month > 2); // take care of February
	$tx = ($ix + ($ix / 4)) % 7 + $day; // take care of leap year
	return ($tx % 7);
}

function daysformonth($month, $year){
    return (30 + ((($month & 9) == 8) || (($month & 9) == 1)) - ($month == 2) - (!((($year % 4) == 0) && ((($year % 100) != 0) || (($year % 400) == 0))) && ($month == 2)));
}

function formatSize($bytes, $precision = 2) { 
    $units = array('MB', 'GB', 'TB'); 

    $bytes = max($bytes, 0); 
    $pow = floor(($bytes ? log($bytes) : 0) / log(1024)); 
    $pow = min($pow, count($units) - 1); 

    // Uncomment one of the following alternatives
    // $bytes /= pow(1024, $pow);
    $bytes /= (1 << (10 * $pow)); 

    return round($bytes, $precision) . ' ' . $units[$pow]; 
}

$outputStream = "";

$i = 0;
$months = array( "January", "Februrary", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December" );

$rcvd = array( 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 );
$sent = array( 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 );

$max = 5;
$smax = 5;
$f = 1;

$totin = 0;
$totout = 0;

if ($month < 1 || $month > 12)
    return $outputStream;

$days = daysformonth($month, $year);
$wd = weekday($month, 1, $year); // first day in month (mon=0, tue=1, ..., sun=6)

for ($tk = 0; $tk < 31; $tk++){
    $sqlDay = "SELECT in_dev, out_dev FROM traffic_info WHERE day = " . mysql_real_escape_string($tk+1) . " AND month = " . mysql_real_escape_string($month) . " AND year = " . mysql_real_escape_string($year) . " AND iface = '" . mysql_real_escape_string($iface) . "' AND UserID = '" . mysql_real_escape_string($_GET['id']) . "';";
    $query = mysql_query($sqlDay, $db_conn);
    if ($row = mysql_fetch_row($query)){
        $rcvd[$tk] = $row[0];
        $sent[$tk] = $row[1];

        $totin = $totin + $row[0];
        $totout = $totout + $row[1];

        if ($rcvd[$tk] > $max)
            $max = $rcvd[$tk];
        if ($sent[$tk] > $max)
            $max = $sent[$tk];
    } else {
        //logWrite("We don't have a one day row...");
    }
}

while ($max > $smax) {
    if ($max > ($f * 5))
        $smax = $f * 10;
    if ($max > ($f * 10))
        $smax = $f * 25;
    if ($max > ($f * 25))
        $smax = $f * 50;
    $f = $f * 10;
}

$devi = 0;
$st = 'MB';

if ($smax > 1000000000){ 
	$devi = 3;
	$smax = (($smax / 1000) / 1000) / 1000;
	$st = 'PB';
} else if ($smax > 1000000){
	$devi = 2;
	$smax = ($smax / 1000) / 1000;
	$st = 'TB';
} else if ($smax > 1000){
	$devi = 1;
	$smax = $smax / 1000;
	$st = 'GB';
}

function formatGlobal($num){
	global $st, $devi;
	for ($i = 0; $i < $devi; $i++){
		$num = $num / 1000;
	}
	return round($num, 2) . ' ' . $st;
}

function ppow($num){
    global $devi;
    for ($i = 0; $i < $devi; $i++){
        $num = $num / 1000;
    }
    return $num;
}

$incom = "Incoming";
$outcom = "Outgoing";

$monthname = $months[$month-1];

?>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
    <html>
        <head>
            <meta http-equiv="Content-Type" content="application/xhtml+xml" />
               <title>Infitialis Traffic Graph</title>

               <script type="text/javascript">
                    //<![CDATA[
                        function Show(label) {
                            document.getElementById("label").innerHTML = label;
                        }
                    //]]>
               </script>

             <style type="text/css">
                #t-graph {position: relative; width: <?= $days * COL_WIDTH ?>px; height: 300px; margin: 1.1em 0 3.5em; padding: 0; border: 1px solid gray; list-style: none; font: 9px Tahoma, Arial, sans-serif;}
                #t-graph ul {margin: 0; padding: 0; list-style: none;}
                #t-graph li {position: absolute; bottom: 0; width: <?= COL_WIDTH ?>px; z-index: 2; margin: 0; padding: 0; text-align: center; list-style: none;}
                #t-graph li.day {height: 298px; padding-top: 2px; border-right: 1px dotted #C4C4C4; color: #AAA;}
                #t-graph li.day_sun {height: 298px; padding-top: 2px; border-right: 1px dotted #C4C4C4; color: #E00;}
                #t-graph li.bar {width: 4px; border: 1px solid; border-bottom: none; color: #000;}
                #t-graph li.bar p {margin: 5px 0 0; padding: 0;}
                #t-graph li.rcvd {left: 3px; background: #228B22;} <?php /* set rcvd bar colour here (green) */ ?>
                #t-graph li.sent {left: 8px; background: #CD0000;} <?php /* set sent bar colour here (red) */ ?>
                <?php
                for ($i = 0; $i < $days - 1; $i++) {
                    ?>
                    #t-graph #d<?= $i+1 ?> {left: <?= $i * COL_WIDTH ?>px;}
                    <?php
                }
                ?>
                #t-graph #d<?= $days ?> {left: <?= ($days - 1) * COL_WIDTH ?>px; border-right: none;}
                #t-graph #ticks {width: <?= $days * COL_WIDTH ?>px; height: 300px; z-index: 1;}
                #t-graph #ticks .tick {position: relative; border-bottom: 1px solid #BBB; width: <?= $days * COL_WIDTH ?>px;}
                #t-graph #ticks .tick p {position: absolute; left: 100%%; top: -0.67em; margin: 0 0 0 0.5em;}
                #t-graph #label {width: 500px; bottom: -20px; z-index: 1; font: 12px Tahoma, Arial, sans-serif; font-weight: bold;}
            </style>
        </head>
        <body>
            <ul id="t-graph">
            <?php
            for ($i = 0; $i < $days; $i++) {
                ?>
                <li class="day<?= ($wd % 7) == 6 ? "_sun" : "" ?>" id="d<?= $i + 1?>" onmouseover="Show('<?= $monthname ?> <?= $i+1 ?>, <?= $year ?> (<?= $incom ?>: <?= formatSize($rcvd[$i]) ?> / <?= $outcom ?>: <?= formatSize($sent[$i]) ?>)')" onmouseout="Show('<?= $monthname ?>, <?= $year ?> (<?= $incom ?>: <?= formatGlobal($totin) ?> / <?= $outcom ?>: <?= formatGlobal($totout) ?>)')"><?= $i+1 ?>
                    <ul>
                        <li class="rcvd bar" style="height: <?= ppow($rcvd[$i]) * 300 / $smax ?>px;"><p></p></li>
                        <li class="sent bar" style="height: <?= ppow($sent[$i]) * 300 / $smax ?>px;"><p></p></li>
                    </ul>
                </li>
                <?php
                $wd++;
            }
            ?>
            <li id="ticks">
            <?php
            for ($i = 5; $i; $i--){ // scale
                ?>
                <div class="tick" style="height: 59px;"><p><?= round($smax * $i / 5,2) ?><?= $smax > 10000 ? " " : "&nbsp;" ?></p></div>
                <?php
            }
            ?>
            </li>

            <li id="label"><?= $monthname ?> <?= $year ?> (<?= $incom ?>: <?= formatGlobal($totin) ?> / <?= $outcom ?>: <?= formatGlobal($totout) ?>)</li>
        </ul>
    </body>
</html>
