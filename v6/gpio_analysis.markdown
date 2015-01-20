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
Im folgenden wird auf die Implementierung des Treibers eingegangen. Dabei wird ein grober Überblick der Funktionen gegeben, welche ausgeführt werden. 
Auf etwaige Fehlerbehandlung wird im Detail nicht eingegangen. Es kann aber davon ausgegangen werden, dass nach jedem kritischen Methodenaufruf, der 
Rückgabewert geprüft wird und ggf. die entsprechende Aktion ausgeführt wird. (Rückgabe eines Fehlerwertes und/oder Abbruch des Programms mit 
anschließendem "Aufräumen" der allozierten Resourcen.) Jedoch werden mögliche Probleme bei dem Programmablauf besprochen und wie diese zu Vermeiden sind.

### Initialisierung
Bei der Initialisierungsfunktion des Treibers wird im Grunde die gleiche Methodik, wie in den zuvor programmierten Treibern, verwendet:
 - Reservierung der Gerätenummer mit `alloc_chredev_region()` mit der automatischen Zuweisung der Gerätenummer
 - Instanzierung und Registrierung eines zeichorientierten Gerätetreiber mit `cdev_alloc()` und `cdev_add()`
 - Registrierung im Sysfs mit `device_create()` und `class_create()`

Zusätzlich wird die Methode `ioremap()` bei der Initialisierung des Treibers verwendet um die physikalische Adresse der GPIO Pins zu erhalten.
Diese Adresse wird später verwendet um an der richtige Stelle bestimmte Werte zu schreiben bzw. davon zu lesen.

Für den Zugriff auf User-Ebene werden die Funktionen Open, Close, Write, Read im Treiber implementiert.

### Öffnen der Datei
Bei der initialisierung des Treibers angelegten Datei, kann nun aus der User-Ebene zugegriffen werden. Öffnet eine Applikation diese Datei
um darin zu schreiben oder zu lesen werden folgende Schritte im Treibercode ausgeführt.
Es wird der GPIO pin 18 als Ausgang und 25 als Eingang konfiguriert. Bei diesem Vorgang muss ein korrektes
Bitmuster in das zugehörige Register geschrieben werden. Dabei wird _zuerst_ das Register ausgelesen und 
die relevanten Bits per UND-Operation gelöscht um danach die Richtung (Aus- oder Eingang)
per ODER-Operation gesetzt. 

Dabei ist zu beachten, dass die richtige Adresse für die jeweiligen Pins vorher ermittelt
wurde. Ebenfalls muss auch das Bitmuster für die Richtung und die Löschung bekannt sein.
All diese Adressen sind in eigenen Makros definiert um unteranderem die Benutzung im Code
zu erleichtern.

Nach dem konfigurieren der Pins ist es dem Benutzer aus der Applikation nun möglich die LED 
anzusteuern indem einfach eine 0 oder 1 in die Datei geschrieben werden kann um die LED aus- 
oder einzuschalten. Das Lesen des aktuellen Zustandes ist ebenfalls möglich.

### Lesen von Werten
Es wird zunächst überprüft ob der Benutzer mehr als ein Byte zum Lesen anfordert, da auch nur 
maximal ein Byte zurückgeliefert werden kann. Ist dies der Fall, wird der Fehlercode 
EAGAIN zurückgegeben. Im Prinzip kann diese Abfrage auch weggelassen werden und einfach immer nur ein
Byte zurückliefern, sie soll aber dazu dienen den Benutzer zu informieren, dass 
generell immer nur ein Byte zurückgeliefert wird.

Es wird also das Register eingelesen und mithilfe von Makros überprüft, welcher Wert
sich darin befindet und das dementsprechende Byte wird zurückgeliefert. ('0' oder '1')


### Schreiben von Werten
Das Schreiben in die Datei funktioniert im Grunde ähnlich wie das Lesen und das 
Initialisieren. Als erstes findet eine Fehlerbehandlung statt, die falsche Benutzereingaben
abfängt. Es sind nur die Werte '1' oder '0' erlaubt.

Danach wird die richtige Adresse ermittelt und das Bitmuster erstellt, welches in das 
Register geschrieben wird. Dieses Bitmuster hängt von dem Wert, welches die Applikation
übergeben hat, hab. 

### Allgemeines
Alle Addressberechnungen werden, wie auch schon die Adressen selbst, in Makros definiert. 
Dies ermöglicht auch unteranderem die Wiederbenutzbarkeit im Code, sowie eine bessere
Lesbarkeit. 

Um gewisse Race Conditions zu vermeiden, wird nach jedem Lesen und Schreiben eine Barrier
eingefügt, um den Compiler daran zu hindern gewisse Code Optimierungen vorzunehmen.

### Schutz kritischer Bereiche
Da unseren Benutzerprogramme rein sequentiell ablaufen und es keine konkurrierende Zugriffe auf das Gerät 
stattfinden, ist der Treiber ohne einen Schutz von kritischen Bereichen implementiert. Der Zugriff ist
sozusagen nicht _thread-safe_. 

Eine Möglichkeit wäre es, die Anzahl der Treiberinstanzen zu beschränken, sodass nur ein Prozess jeweils 
über diesen Treiber auf die LED zugreifen kann. _(vgl. V5)_ Damit ist zwar gewährleistet, dass die Datei nur einmal
geöffnet werden kann, jedoch nicht dass mehrere Threads (der gleichen Applikation) aus der Datei lesen oder in sie schreiben.
Schon gar nicht ist damit ausgeschlossen, dass eventuelle andere Treiber ebenfalls gleichzeitig auf die Hardware zugreifen.

Um den erstgenannten Schutz zu gewährleisten, muss der Benutzer in der Anwendungsebene gewisse Mechanismen selbst 
implementieren. Um also konsistente Daten zu erhalten, sollte der Benutzer seine Schreib- oder Lesezugriffe mit z.B. Mutexen schützen. (o.ä.
siehe Betriebssysteme-Vorlesung) 


