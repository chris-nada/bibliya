# Bibliya

**Bibliya ist freie Software zum Anzeigen von Bibeltexten.**

![bildschirmfoto](/screenshot.png)

+ Es können mehrere Bibelübersetzungen zur Textanalyse nebeneinander verglichen werden.
+ Die Übersetzungen werden aus OSIS-Dateien eingelesen, **einige freie Übersetzungen\* sind bereits enthalten**. 
+ Weitere Übersetzungen sind hier: https://github.com/gratis-bible/bible oder hier: https://ebible.org/find/ frei erhältlich.
    + Die heruntergeladene (OSIS-) XML-Datei muss in den Ordner `bibliya/data/de/` gelegt werden. (Für andere Sprachen ggf. anderen Ordner wählen oder Ordner erstellen).
+ Die Nutzeroberfläche ist derzeit fest auf Deutsch eingestellt.
+ Wünsche zu neuen Funktionen, Fragen oder Fehler sehr gern an mich oder hier ein _Issue_ eröffnen.

# Download
**Portable Zip-Datei. Einfach irgendwo entpacken und mit `bibliya.exe` starten.**

### Version 2021_05_27
+ **Windows** [Download](https://drive.google.com/file/d/1kOCIo6S2bGZabPln4Pl_9pqKG59c7-cF) (44MB, Google Drive)
  + Neu: Tabs
  + Neu: Pfeiltasten zur Navigation Vers/Kapitel, Pos1 = Vers1
  + Neu: Standardmäßig 3 englische Übersetzungen hinzugefügt
  + Etc: ComboBox rastet auf aktuelles Buch ein
  + Etc: Standardmodus ist jetzt Kapitel (statt 1 Vers / 5 Verse)
  + Etc: Fenstermodus ist jetzt standard (um Systemleiste nicht zu verstecken)
  + Fix: CPU Auslastung verringert
  + Fix: Suchen-Scroller sprang bei Klick auf 'Anzeigen'
  + Fix: Aktuelles Buch korrekt speichern

### Version 2021_01_13
+ **Windows** [Download](https://drive.google.com/file/d/1bYWOMPRbtVXzT85tj15iTiYK9iQTUN-f) (27MB, Google Drive)
  + Neu: Einstellungen für Farben und Schriftgröße
  + Neu: Verse beginnen auf gleicher Höhe
  + Neu: Anzeigbare historische Karte von Palästina
  + Fix: OSIS-Import weiter verbessert: Fehlende Texte aus `<p>` - Tags
  + Fix: Race Conditions beim Import
  + Fix: Laden map.at nach gelöschter Übersetzung

### Version 2020_12_20
+ **Windows** [Download](https://drive.google.com/file/d/19nk4hd9sUFcNDow1cXqgNTKCZw8h7WGe) (25MB, Google Drive)
  + Neu: Fenster Minimieren
  + Neu: Arbeitsstand wird gespeichert
  + Neu: Lesezeichen
  + Neu: Icons + Tooltips
  + Neu: Suche
  + Neu: Import aus CSV (Exportiert aus SQLite3)
  + Neu: Unterstützung weiterer Glyphen (Zeichensätze)
  + Etc: Konsolenfenster versteckt
  + Etc: Verbesserter OSIS-Import
  + Fix: Mehrere Übersetzung mit gleichen Namen in versch. Sprachen möglich
  + Fix: Absturz bei der Übersetzungsauswahl

### Version 2020_12_18
+ **Windows** [Download](https://drive.google.com/file/d/1AbgRRBiBqjHNMqs08IzDzE_lexQ1w3nX) (16MB, Google Drive)
  + Erste Veröffentlichung
  
# Systemanforderungen
(für obige herunterladbare Versionen. Falls der Quelltext selbst kompiliert wird, können die Anforderungen abweichen)
+ **PC von etwa 2007 oder neuer.**
+ RAM-Nutzung ist bei 10 importieren Übersetzungen bei etwa 200-300 MB.
+ Windows-Version: 64-Bit-Edition von Windows XP, 7, 8, 8.1 oder 10.
+ Linux-Version: Eine 64-Bit-Version, die auf Debian basiert (z.B. Ubuntu, MXLinux etc.).

# Lizenz
+ [BSD 3-Klauseln](LICENSE)
