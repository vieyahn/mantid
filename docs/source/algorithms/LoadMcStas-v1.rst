.. algorithm::

.. summary::

.. alias::

.. properties::

Description
-----------

Reads a McStas Nexus file into a Mantid WorkspaceGroup with a 
user-supplied name. Data generated by McStas monitor components are 
stored in workspaces of type Workspace2D or Event.

For further information about Mcstas and Mantid see `here <https://github.com/McStasMcXtrace/McCode/wiki/McStas-and-Mantid>`_.
This includes how to generate a McStas 2.1 event data file and the corresponding IDF.
It also includes McStas event data conventions.
The information in the link is *work in progress*.

LoadMcStas replaces LoadMcStasEventNexus. LoadMcStas can be used for 
reading McStas 2.1 histogram and event data. 
:ref:`algm-LoadMcStasNexus` can be used 
for reading McStas 2.0 histogram data. 

LoadMcStas will be called by Load, if the file has an 
``/entry1/simulation/name`` item whose value is ``"mccode"``.

The output workspace will contain one workspace for each group of
class ``NXdata`` and not of name ``"content_nxs"`` found in a  
group of class ``NXDetector`` of name ``"Data"``.
The name of the workspace is the same as the name of the group, 
but with ``__mcstas_event_hist`` added to the end.

+----------------------------------+------------------------------------------+---------------------------------------+
| Description of Data              | Found in Nexus file (within 'run')       | Placed in Workspace (Workspace2D)     |
|                                  |                                          | or output                             |
+==================================+==========================================+=======================================+
| All data                         | Group of class ``NXDetector``            | See below                             |
|                                  | of name ``"data"``                       |                                       |
+----------------------------------+------------------------------------------+---------------------------------------+
| Generic group including either   | each group of class ``NXdata`` in        | one workspace each                    |
| event or histrogram data         | ``"data"``, henceforth referred to as    |                                       |
|                                  | [DATA]                                   |                                       |
+----------------------------------+------------------------------------------+---------------------------------------+
| Event data                       | item in a [DATA] with a ``long_name``    | event data                            |
|                                  | attribute containing ``"Neutron_ID"``    |                                       |
|                                  | and name ``"events"``                    |                                       |
+----------------------------------+------------------------------------------+---------------------------------------+
| Histrogram data                  | items in a [DATA] with a ``long_name``   | histogram data                        |
|                                  | attribute **not** containing             |                                       |
|                                  | ``"Neutron_ID"``                         |                                       |
+----------------------------------+------------------------------------------+---------------------------------------+
| Instrument                       | ``/instrument``                          | loaded into workspace, only if        |
|                                  |                                          | events are loaded                     | 
+----------------------------------+------------------------------------------+---------------------------------------+
| Instrument definition            | ``/instrument/instrument_xml/data``      | loaded into workspace, only if        |
|                                  | is needed for events to be loaded        | events are loaded                     | 
+----------------------------------+------------------------------------------+---------------------------------------+

The event data of the McStas file occurs in a table with six columns:

1. Weight
2. X coordinate
3. Y coordinate
4. Number of previous events
5. Detector ID
6. Time

The **ErrorBarsSetTo1** property applies to event data, but not to histogram data.


References
##########

For more information about McStas and its general usage for simulating neutron 
scattering instruments and experiments visit the McStas homepage http://www.mcstas.org.


.. categories::

.. sourcelink::
