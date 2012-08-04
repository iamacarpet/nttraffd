<?php

$db_user = '';
$db_pass = '';
$db_host = 'localhost';
$db_name = '';

$db_conn = mysql_pconnect($db_host, $db_user, $db_pass) or die('01');
mysql_select_db($db_name, $db_conn) or die('02');

if (!$_GET['clientid']){
    die('03');
} else if (!$_GET['authkey']){
    die('04');
} else {
    // Check Client ID and Auth Key
    $sql = "SELECT ID FROM user_info WHERE ID = '" . mysql_real_escape_string($_GET['clientid']) . "' AND ServerKey = '" . mysql_real_escape_string($_GET['authkey']) . "'";
    $query = mysql_query($sql, $db_conn) or die('05');
    if (!$row = mysql_fetch_row($query)){
        die('06');
    }
}

if (!$_GET['iface']){
    die('07');
}

if (!$_GET['year']){
    die('08');
}

if (!$_GET['month']){
    die('09');
}

if (!$_GET['day']){
    die('010');
}

if (@$_GET['in'] == ""){
    die('011');
}

if (@$_GET['out'] == ""){
    die('012');
}

// Check if we have an existing row for this data...
$sql = "SELECT ID, in_dev, out_dev FROM traffic_info WHERE UserID = '" . mysql_real_escape_string($_GET['clientid']) . "' AND iface = '" . mysql_real_escape_string($_GET['iface']) . "' AND year = '" . mysql_real_escape_string($_GET['year']) . "' AND month = '" . mysql_real_escape_string($_GET['month']) . "' AND day = '" . mysql_real_escape_string($_GET['day']) . "'";
$query = mysql_query($sql, $db_conn) or die('013');
if ($row = mysql_fetch_row($query)){
    // Update existing row...
    $sql2 = "UPDATE traffic_info SET in_dev = " . mysql_real_escape_string(intval($_GET['in'])+$row[1]) . ", out_dev = " . mysql_real_escape_string(intval($_GET['out'])+$row[2]) . " WHERE ID = '" . mysql_real_escape_string($row[0]) . "'";
} else {
    // Create row...
    $sql2 = "INSERT INTO traffic_info (UserID, iface, year, month, day, in_dev, out_dev) VALUES ('" . mysql_real_escape_string($_GET['clientid']) . "', '" . mysql_real_escape_string($_GET['iface']) . "', '" . mysql_real_escape_string($_GET['year']) . "', '" . mysql_real_escape_string($_GET['month']) . "', '" . mysql_real_escape_string($_GET['day']) . "', '" . mysql_real_escape_string($_GET['in']) . "', '" . mysql_real_escape_string($_GET['out']) . "')";
}

$query2 = mysql_query($sql2, $db_conn) or die('014');

echo '1';

?>
