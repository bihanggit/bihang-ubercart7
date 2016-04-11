<?php

require_once("/lib/Bihang.php");

function uc_bihang_notification()
{
    $client = Bihang::withApiKey(variable_get('uc_bihang_api_key',''),variable_get('uc_bihang_api_secret',''));
    if( $client->checkCallback() ){
        $bihang_order = json_decode(file_get_contents("php://input"));
        if( $bihang_order ){
            $order_status = db_query("SELECT order_status FROM {uc_orders} WHERE order_id = :order_id", array(':order_id' => $bihang_order->custom))->fetchField();
            $uid          = db_query("SELECT uid FROM {uc_orders} WHERE order_id = :order_id", array(':order_id' => $bihang_order->custom))->fetchField();
            if ( (($order_status != 'canceled') && ($order_status != 'completed')) &&  $bihang_order->status === 'completed' )
            {
                uc_order_update_status($bihang_order->custom, 'payment_received');
                uc_payment_enter($bihang_order->custom, 'bihang', $bihang_order->total_native->amount, $uid, NULL, '', time());
                db_query("DELETE FROM {uc_payment_bihang} WHERE order_id = :order_id", array(':order_id' => $bihang_order->custom));
            }
            header('HTTP/1.0 200 OK');
        }
    }
} // end function - uc_bihang_notification
