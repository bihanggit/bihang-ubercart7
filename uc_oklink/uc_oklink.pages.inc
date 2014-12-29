<?php

require_once("/lib/Oklink.php");

function uc_oklink_notification()
{
    $client = Oklink::withApiKey(variable_get('uc_oklink_api_key',''),variable_get('uc_oklink_api_secret',''));
    if( $client->checkCallback() ){
        $oklink_order = json_decode(file_get_contents("php://input"));
        if( $oklink_order ){
            $order_status = db_query("SELECT order_status FROM {uc_orders} WHERE order_id = :order_id", array(':order_id' => $oklink_order->custom))->fetchField();
            $uid          = db_query("SELECT uid FROM {uc_orders} WHERE order_id = :order_id", array(':order_id' => $oklink_order->custom))->fetchField();
            if ( (($order_status != 'canceled') && ($order_status != 'completed')) &&  $oklink_order->status === 'completed' )
            {
                uc_order_update_status($oklink_order->custom, 'payment_received');
                uc_payment_enter($oklink_order->custom, 'oklink', $oklink_order->total_native->amount, $uid, NULL, '', time());
                db_query("DELETE FROM {uc_payment_oklink} WHERE order_id = :order_id", array(':order_id' => $oklink_order->custom));
            }
            header('HTTP/1.0 200 OK');
        }
    }
} // end function - uc_oklink_notification
