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

