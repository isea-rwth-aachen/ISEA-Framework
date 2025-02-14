Addition simulink libraries       {#bibs}
==========
\~German

Für Simulink stehen zusätzliche Bibliotheken bereit. Mithilfe dieser Blöcke ist es möglich, einfache Balancing Strategien zu verwirklichen.
Teil dieser Bibliothek sind folgende Strategien:
- Passives Balancing
- Nullstratgie
- Aktives Balancing

![Balancing Strategien](balancing_strategies.png)

Wichtig ist bei diesen Strategien, dass Energie aus dem System entnommen oder zusätzliche Energie hinzugeführt werden kann. Daher es ist keine Energieneutralität gegeben.
Dies muss insbesondere beim aktiven Balancing beachtet werden. Hier gilt nicht, dass die Summe aller Balancingströme gleich null ist ( \f$\sum_{k} i_{balancing,k} = 0 \f$ ), wie es in der Realität eigentlich sein sollte.
Die Überprüfung dieser Größen obliegt dem Anwender, da dieser auch aus anderen, dem System nicht bekannten Energiequellen Energie einspeisen könnte.
Bei Parallelschaltung von Batterien und dem Überlagern von Strömen, bei aktivem oder passivem Balancing, werden diese auf die parallelen Zweige verteilt, so wie es physikalisch sinnvoll ist (siehe Bild Balancing Überlagerung).

![Balancing Überlagerung](balancing_schematic.png)
Es sollte immer eine Strategie an den Balancingport angeschlossen sein, da es ansonsten zu Fehlern kommt.


Passives Balancing
------
![Passives Balancing](passive_strategies.png)

Die passive Strategie hat drei Eingangsgrößen
-# Spannung ab der gebalanced werden soll (\f$U_{bal}\f$)
-# Spannungen aller Batterien, die gebalanced werden sollen (*U*)
-# Ohmscher Widerstand der zum balancen genutzt werden soll (*R*)

Die passive Strategie hat eine Ausgangsgröße:
-# Balancingstrom, der den Batterien überlagert wird

Sobald eine Spannung der zu balancenden Batterien größer als der Grenzwert ist, wird eine zusätzliche Masche über die Batterie gelegt.
<br/>
Für \f$ U \geq U_{bal}\f$:
\f[
i_{balancing} = -\frac{U}{R}
\f]

ansonsten
\f[
i_{balancing} = 0
\f]

Diese Strategie empfiehlt sich z.B. dann, wenn wie beim Wima Supercappack schädigende Überspannung verhindert werden soll. Die Energie, die am Widerstand in thermische Energie umgewandelt wird, nimmt keinen Einfluss auf das System mehr.


Nullstratgie
------
![Nullstrategie](null_strategies.png)

Die Nullstrategie hat eine Eingangsgrößen
-# Spannungen aller Batterien die gebalanced werden sollen (*U*)

Die Nullstrategie hat eine Ausgangsgröße:
-# Balancingstrom der den Batterien überlagert wird

Bei der Nullstrategie wird kein Strom der Batterie hinzu- oder abgeführt und ist damit neutral. Beim ausgegebenen Vektor sind alle Einträge Null.
Der Eingang der Spannung ist wichtig, um immer einen Stromvektor mit der richtigen Größe bestimmen zu können.

Aktives Balancing
------
![Aktives Balancing](active_strategies.png)
Das aktive Balanancing eröffnet die Möglichkeit jeder Batterie einen Strom zu überlagern, hierbei gilt jedoch die im oberen beschriebene Einschränkung bei Parallelschaltungen.
Beim aktiven Balancing ist es dem Benutzer überlassen für die Energieneutralität im Gesamtsystem zu sorgen.


\~English
Additional libraries are provided for a simulink model.
These libraries can be used as templates for creating own balancing strategies.
The following basic strategies are provided:
- passive balancing
- null strategy
- active balancing

![Balancing strategies](balancing_strategies.png)

When using balancing it is important to note, that it is possible to create an energy imbalance inside the electrical model.
Energy neutrality is not enforced as there might be different energy systems outside of the electrical model which supply additional energy.
This is especially true for active balancing.
The current of all balancing currents need not to be \f$\sum_{k} i_{balancing,k} = 0 \f$, which would be true in reality.
The consistency check should be done by the user.
If several batteries are connected in parallel the balancing current will be split into every branch according to Kirchoff's circuit law (see picture below).

![Superposition of balancing currents](balancing_schematic.png)
A balancing strategy should be *always* connected, otherwise errors in the model might arise.


Passive balancing
------
![Passive balancing](passive_strategies.png)

The passive simulink block has three inputs
-# Balancing starting voltage (\f$U_{bal}\f$)
-# Voltage of the batteries which should be balanced (*U*)
-# Ohmic resistance for balancing (*R*)

The following output are defined
-# Balancing current, which are applied to the battery

When the balancing is active the following additional equation is present.
<br/>
For \f$ U \geq U_{bal}\f$:
\f[
i_{balancing} = -\frac{U}{R}
\f]

otherwise
\f[
i_{balancing} = 0
\f]


Null strategy
------
![Nullstrategie](null_strategies.png)

The null strategy has the following input defined
-# Voltage of the batteries which should be balanced (*U*)

The following output are defined
-# Balancing current, which are applied to the battery

The null strategy does always return a zero vector as balancing current and therefore no balancing takes place.
The input voltage is of importance as this is used to derive the size of the output vector.

Active Balancing
------
![Active Balancing](active_strategies.png)

Active balancing enables one to superposition any current desired, but with the explained consistency condition.

