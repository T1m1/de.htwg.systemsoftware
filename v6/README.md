# HowTo

Für den Versuch V6 wird der Source-Code aus der Übungsaufgabe V3 benötigt. In dieser Beschreibung wird von einem neu ausgecheckten (clone) Repositroy ausgegangen.

## Vorbereitung
Zu Beginn muss der Buildroot Ordner neu **ausgecheckt** und für die Verwendung der Buildroot Pakete von **V6 vorbereitet** werden. Dies geschicht mit folgendem Befehl:
```sh
$ ./v6.sh -init
```

An diesem Punkt muss das **Buildroot Menü** aufgerufen werden.
```sh
$ ./v6.sh -m
```
Im Menü müssen an folgender Stelle die gewünschten Pakete **aktiviert** werden. 

```sh
Target packages -> Miscellaneous -> syso -> Modules -> <name>
```

Nach Auswahl der Pakete müssen die Änderungen beim Beenden des Menüs **gespeichert** werden. Jetzt kann das **compilieren** der Sourcen starten. 
```sh
$ ./v6.sh -co
```
Im Anschluss müssen die generierten Dateien noch auf das Netzlaufwerk für den Raspberry PI **kopiert** werden.
```sh
$ ./v6.sh -cp
```

Nach einem Neustart des Raspberry Pi sollte die Aufgabe V6 getestet werden können.

Viel Spaß!
