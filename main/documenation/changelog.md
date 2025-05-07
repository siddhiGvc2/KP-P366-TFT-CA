## 050525
-- use static QR code
-- set QR code using *QR:QRstring# command
-- interface with Mobivend APIs
-- interface with GVC Vending Board (via UART)
-- set Uart baud rate to 9600
-- command to UART when getting payment *VEND,PAYTM,TID,Amount,GUID#
-- send *ARD+ESP# when device powered on before trying for wifi
-- send z after some delay
-- send *CONFIGOK# after some delay
-- send *OKNET# when wifi and mqtt connected
- when UART sends *TRXN,POSID,TID,SID,PAYTM,Price,SprialNumber# - this means this item has been vend, send API to mobivend server.
-- send Serial Number via *PID,xxxx# to UART
-- serial number is without NA-1507-
-- MS? & WS? command added in uart

#060525
-- MS? & WS? command added in uart
-- replay as MSx and WSx
-- send *WiFi,SSID# when wifi connected
-- send *WS1# when wifi connected and *WS0# when wifi disconnected
-- send *MS1# when MQTT connected and *MS0# when mqtt disconnected
-- send *Dddmmyyhhmmss# when wifi connected
-- remove unnecessary UART command
-- when *65121,PAYTM,AmountReceived#  received, reply with *SUCCESS# to UART
-- do not send this or any UART packet to TCP

## 070525
-- change QR code to thaat of Mobivend (done 12:30 - 12:45 )
-- ensure that QR code is changed when new value is set (pending)
-- removed ESP_LOGI (for MQTT HBT) (12:45 - 12:50)
-- stop calling mobivend APIs (pending)
when *65121,PAYTM,AmountReceived#  received, reply with *SUCCESS# to UART
-- do not send this or any UART packet to TCP





