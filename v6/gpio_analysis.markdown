# GPIO Zugriff

## Ausgangssituation
Über einen Raspberry pi ist eine LED angeschloseen, welche sich sowohl über das 
Sys-Filesystem als auch über einen eigens geschriebenen Treiber ansprechen
lässt. Um die LED blinken zu lassen, werden aus der User-Ebene die Werte 0 oder 
1 in eine entsprechende Datei geschrieben.

## Maximale Frequenz
Es soll nun bestimmt werden mit welcher maximalen Frequenz auf diese Datei (und
demnach auf die LED) zugegriffen werden kann. Es soll dabei der Zugriff zwischen
dem Treiber und dem Sys-Filesystem verglichen werden. Bei der Berechnung der 
maximalen Frequenz wird **1 Millionen Mal** die LED aus- und angeschaltet. Dabei 
wird davor und danach die Zeit gemessen, welche die Grundlage für die Frequenzberechnung dient.
Dieser Vorgang wird fünf Mal wiederholt. Sowohl für den Treiber, als auch für das Sys-Filesystem.

_Hinweis: Bei der Berechnung der Frequenz wird die Periode eines Blinkzyklus 
verwendet. Eine Periode ist also: LED-OFF, LED-ON_

### Ergebnis Treiber
Folgende Messergebnisse ergaben sich:
 - 501,721 kHz
 - 500,478 kHz
 - 500,483 kHz
 - 500,480 kHz
 - 500,471 kHz

Maximale Frequenz: **501,721 kHz**

Durchschnitt: **500,727 kHz**

Daraus ergibt sich eine Treiberzugriffsgeschwindigkeit von **0,9966 µs** (1,993 µs pro Periode)

