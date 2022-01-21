# Rapport pour synchronisation pour l'IOT

## Mise en place

D'abord lancer Arduino avec la commande : arduino185 &
Puis dans Arduino >> Type de carte : Sélectionner Teensy 3.2/3.1

## Prise en main du transceiver UWB

### Prise en main des sketchs exemples

DecaDuinoSender : 
	MAX_FRAME_LEN 120 : Nombre d'éléments à envoye
	txData : Data qui seront envoyés sur le support
	txLen : Non utilisé
	decaduino : Objet pour interagir avec la carte (init, dataRequest, ...)
	rxFrames : Nombre de frames reçus

	Ce programme commence par l'initialisation du port série, du bus SPI et de la carte Decaduino.
	Si l'init de la carte échoue, la led 13 clignote toutes les 50ms
	Ensuite, toutes les secondes, la carte Decaduino envoie une trame sur le support.

DecaDuinoReceiverSniffer :
	MAX_FRAME_LEN 120 : Nombre d'éléments max en réception
	rxData : Data qui seront reçus sur le support
	rxLen : Tailles du paquet reçus
	decaduino : Objet pour interagir avec la carte (init, setRxBuffer, ...)

	Ce programme commance comme le précédent.
	Ensuite elle met en place place le buffer de réception, puis fait une requếte sur le support
	Enfin, dès qu'une trame est disponible, ce programme affiche les différentes informations reçus et relance la requête pour obtenir un nouveau paquet.


Adressage dans les messages 


