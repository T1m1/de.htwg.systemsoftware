# GPIO Zugriff

## Ausgangssituation
Über einen Raspberry pi ist eine LED angeschloseen, welche sich sowohl über das 
Sys-Filesystem als auch über einen eigens geschriebenen Treiber ansprechen
lässt. Um die LED blinken zu lassen, werden aus der User-Ebene die Werte 0 oder 
1 in eine entsprechende Datei geschrieben. Im ersten Teil wird die Zugriffsgeschwindigkeit bestimmt und der zweite Teil beschäftigt sich mit
dem selbst geschriebenen Treiber.

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

_Anmerkung: Bei mehrmaligen Testdurchläufen, ergab sich in unregelmäßigen Abständen ein Wert um ca. 1 MHz. Dieses Verhalten konnte jedoch nicht
gezielt reproduziert werden, sondern trat vollkommen zufällig auf._

### Ergebnis Sys-Filesystem
Bei dem Test über das Sys-Filesystem ergaben sich folgende Messwerte
 - 168,032 kHz
 - 198,166 kHz
 - 168,032 kHz
 - 198,165 kHz
 - 168,033 kHz

Maximale Frequenz: **198,165 kHz**

Durchschnitt: **180,086 kHz**

Daraus ergibt sich eine Zugriffsgeschwindigkeit von **2,523 µs** (5,046 µs pro Periode)

### Interpretation
Man kann deutlich erkennen, dass der Treiber eine höhere Zugriffsgeschwindigkeit aufweist. 
Die gemessenen Werte lassen erkennen, dass mit mehr als der doppelten Geschwindigkeit die LED blinkt. Genauer: **2,53 mal** schneller.
Bei dieser Berechnung wurden die jeweils maximalen Werte herangezogen. Wenn man die Rechnung mit den Durchschnittswerten durchführt, ist
der Treiberzugriff etwas höher: **2,78 mal**.
Da die Zugriffszeit auf das Sys-Filesystem aber bis zu 250 mal* langsamer sein kann, sind diese Werte zwar gut aber nicht perfekt.

_*) Aus Vorbereitung von V6_

## Treiberdisskusion

