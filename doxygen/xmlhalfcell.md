Half cell simulation     {#xmlhalfcell}
====================

\~German

Mit der Halbzellensimulation wird eine elektrodenaufgelöste Beschreibung einer Batteriezelle gegeben. Das elektrische Ersatzschaltbild wird für die Halbzellensimulation dahingehend erweitert, dass die Ruhespanung der Vollzelle über eine Superposition der Potentialwerte von anodischer und kathodischer Halbzelle abgebildet wird. Zusäztlich erfolgt eine anoden- und kathodenseitige Zerlegung der Zellimpedanz.

<br/>

XML-Aufbau
==========

\htmlinclude halfcell_example.xml

<**Anode**> Über diesen Knoten werden alle elektrischen Bauelemente gegeben, die die Anode beschreiben.

<**AnodeElement**> Jedes Element mit dem Attribut "class=AnodeElement" beschreibt einen einzelnen Anodenpartikel, dem ein eigener SOC zur Beschreibung des Lithiierungsgrades zugewiesen wird. Die Kapazität der Vollzelle wird dabei gleichmäßig über die einzelnen Anodenpartikel verteilt. In gegebenen Beispiel wird die Anode der Zelle durch die parallele Verschaltung zweier Anodenpartikel beschrieben.

<**Cathode**> Über diesen Knoten werden alle elektrischen Bauelemente gegeben, die die Kathode beschreiben.

<**CathodeElement**> Jedes Element mit dem Attribut "class=CathodeElement" beschreibt einen einzelnen Kathodenpartikel, dem ein eigener SOC zur Beschreibung des Lithiierungsgrades zugewiesen wird. Die Kapazität der Vollzelle wird analog zur Anode gleichmäßig über die einzelnen Kathodenpartikel verteilt. In gegebenen Beispiel wird die Kathode der Zelle durch einen einzelnen Kathodenpartikel beschrieben.

Eine ausführbare Beispieldatei für die Halbzellensimulation findet sich unter scenarions/thermal_electrical/tesla_halfcell_color.xml

\~English

The half cell simulation enables a description of a battery cell that is aware of the electrodes. The ECM is altered so that the open circuit voltage of the full cell is a superposition of anode and cathode potential. The cell's impedance is also diviced into anode and cathode.

<br/>

XML structure
==========

\htmlinclude halfcell_example.xml

<**Anode**> This element contains all electrical elements describing the anode

<**AnodeElement**> Each element with the class "AnodeElement" describt a single anode particle with its own SOC to describe the degree of lithiation. The capacity of the cell is equally distributed among all anode particles. In the example above, the anode is described by two anode particles that are connected in parallel.

<**Cathode**> This element contains all electrical elements describing the cathode

<**CathodeElement**> Each element with the class "CathodeElement" describt a single cathode particle with its own SOC to describe the degree of lithiation. The capacity of the cell is equally distributed among all cathode particles. In the example above, the cathode is described by a single particle.

An in-depth example for a half cell simulation can be found in scenarions/thermal_electrical/tesla_halfcell_color.xml
