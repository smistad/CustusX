#include "cxStateMachineManager.h"

#include <iostream>
#include <QByteArray>
#include "sscXmlOptionItem.h"
#include "cxDataLocations.h"
#include "cxPatientData.h"

namespace cx
{
Desktop::Desktop()
{}

Desktop::Desktop(QString layout, QByteArray mainwindowstate) :
    mLayoutUid(layout),
    mMainWindowState(mainwindowstate)
{}

class ApplicationsParser
{
public:
  ApplicationsParser()
  {
    mXmlFile = ssc::XmlOptionFile(DataLocations::getXmlSettingsFile(),"CustusX").descend("applications");

    //Lab application
    this->addDefaultDesktops("PatientDataUid", "LAYOUT_3DACS_2X2", "AAAA/wAAAAD9AAAAAgAAAAAAAAGBAAADifwCAAAAC/sAAAAUAEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAALAAAAqYAAAJCAP////sAAAAiAEMAbwBuAHQAZQB4AHQARABvAGMAawBXAGkAZABnAGUAdAEAAAMgAAAAWgAAAAAAAAAA+wAAADIASQBtAGEAZwBlAFAAcgBvAHAAZQByAHQAaQBlAHMARABvAGMAawBXAGkAZABnAGUAdAEAAAM0AAAAuwAAAAAAAAAA+wAAABQARABvAGMAawBXAGkAZABnAGUAdAEAAAN7AAAAYAAAAAAAAAAA+wAAADwATQBlAHMAaABQAHIAbwBwAGUAcgB0AGkAZQBzAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAALAAAAW0AAACIAP////sAAABGAFIAZQBnAGkAcwB0AHIAYQB0AGkAbwBuAEgAaQBzAHQAbwByAHkAVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAA/////wAAAGAA/////AAAABoAAAOJAAABkAEAABT6AAAAAQEAAAAC+wAAAEAAVAByAGEAbgBzAGYAZQByAEYAdQBuAGMAdABpAG8AbgBXAGkAZABnAGUAdABEAG8AYwBrAFcAaQBkAGcAZQB0AQAAAAD/////AAAA7QD////7AAAALgBCAHIAbwB3AHMAZQByAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQBAAAAAAAAAYEAAAChAP////sAAAA+AFMAaABpAGYAdABDAG8AcgByAGUAYwB0AGkAbwBuAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAAAP////8AAAAyAP////sAAABCAEkAbQBhAGcAZQBSAGUAZwBpAHMAdAByAGEAdABpAG8AbgBXAGkAZABnAGUAdABEAG8AYwBrAFcAaQBkAGcAZQB0AAAAACwAAAS0AAABNwD////7AAAARgBQAGEAdABpAGUAbgB0AFIAZQBnAGkAcwB0AHIAYQB0AGkAbwBuAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAAAP////8AAAF/AP////sAAAA0AE4AYQB2AGkAZwBhAHQAaQBvAG4AVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAA/////wAAAOsA////AAAAAgAABpAAAACw/AEAAAAD+wAAADwAVABvAG8AbABQAHIAbwBwAGUAcgB0AGkAZQBzAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAAAAAAAXYAAAF2AP////sAAAA6AFAAbwBpAG4AdABTAGEAbQBwAGwAaQBuAGcAVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAAAAAC2wAAASYA////+wAAAD4ASQBtAGEAZwBlAFAAcgBvAHAAZQByAHQAaQBlAHMAVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAAAAAGkAAAAIsA////AAAFDgAAA4kAAAAEAAAABAAAAAgAAAAI/AAAAAEAAAACAAAABQAAABYARABhAHQAYQBUAG8AbwBsAEIAYQByAAAAAAD/////AAAAAAAAAAAAAAAWAFQAbwBvAGwAVABvAG8AbABCAGEAcgAAAAAA/////wAAAAAAAAAAAAAAIgBOAGEAdgBpAGcAYQB0AGkAbwBuAFQAbwBvAGwAQgBhAHIAAAAAAP////8AAAAAAAAAAAAAAB4AVwBvAHIAawBmAGwAbwB3AFQAbwBvAGwAQgBhAHIBAAAAAP////8AAAAAAAAAAAAAABwARABlAHMAawB0AG8AcABUAG8AbwBsAEIAYQByAQAAAvb/////AAAAAAAAAAA=");
    this->addDefaultDesktops("NavigationUid", "LAYOUT_3DACS_2X2", "AAAA/wAAAAD9AAAAAgAAAAAAAAGBAAAEOPwCAAAADPsAAAAUAEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAALAAAArQAAAJCAP////sAAAAiAEMAbwBuAHQAZQB4AHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAsAAABLAAAAAAAAAAA+wAAADIASQBtAGEAZwBlAFAAcgBvAHAAZQByAHQAaQBlAHMARABvAGMAawBXAGkAZABnAGUAdAEAAAM0AAAAuwAAAAAAAAAA+wAAABQARABvAGMAawBXAGkAZABnAGUAdAAAAAAsAAABvwAAAAAAAAAA+wAAADwATQBlAHMAaABQAHIAbwBwAGUAcgB0AGkAZQBzAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAALAAABDgAAACIAP////sAAABGAFIAZQBnAGkAcwB0AHIAYQB0AGkAbwBuAEgAaQBzAHQAbwByAHkAVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAA/////wAAAGAA////+wAAAC4AQgByAG8AdwBzAGUAcgBXAGkAZABnAGUAdABEAG8AYwBrAFcAaQBkAGcAZQB0AAAAAAD/////AAAApgD////7AAAAQABUAHIAYQBuAHMAZgBlAHIARgB1AG4AYwB0AGkAbwBuAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAAAP////8AAAF7AP////sAAAA+AFMAaABpAGYAdABDAG8AcgByAGUAYwB0AGkAbwBuAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAAAP////8AAAAyAP////sAAABCAEkAbQBhAGcAZQBSAGUAZwBpAHMAdAByAGEAdABpAG8AbgBXAGkAZABnAGUAdABEAG8AYwBrAFcAaQBkAGcAZQB0AAAAAAD/////AAABNwD////7AAAARgBQAGEAdABpAGUAbgB0AFIAZQBnAGkAcwB0AHIAYQB0AGkAbwBuAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAAAP////8AAAF/AP////sAAAA0AE4AYQB2AGkAZwBhAHQAaQBvAG4AVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAA/////wAAAOsA////AAAAAgAABpAAAACw/AEAAAAD+wAAADwAVABvAG8AbABQAHIAbwBwAGUAcgB0AGkAZQBzAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAAAAAAAXYAAAF2AP////sAAAA6AFAAbwBpAG4AdABTAGEAbQBwAGwAaQBuAGcAVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAAAAAC2wAAASYA////+wAAAD4ASQBtAGEAZwBlAFAAcgBvAHAAZQByAHQAaQBlAHMAVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAAAAAGkAAAAIsA////AAAGkAAAA4kAAAAEAAAABAAAAAgAAAAI/AAAAAEAAAACAAAABQAAABYARABhAHQAYQBUAG8AbwBsAEIAYQByAAAAAAD/////AAAAAAAAAAAAAAAWAFQAbwBvAGwAVABvAG8AbABCAGEAcgEAAAAA/////wAAAAAAAAAAAAAAIgBOAGEAdgBpAGcAYQB0AGkAbwBuAFQAbwBvAGwAQgBhAHIAAAAAAP////8AAAAAAAAAAAAAAB4AVwBvAHIAawBmAGwAbwB3AFQAbwBvAGwAQgBhAHIBAAAAav////8AAAAAAAAAAAAAABwARABlAHMAawB0AG8AcABUAG8AbwBsAEIAYQByAQAAA2D/////AAAAAAAAAAA=");
    this->addDefaultDesktops("ImageRegistrationUid", "LAYOUT_3DACS_2X2", "AAAA/wAAAAD9AAAAAgAAAAAAAAGBAAADifwCAAAADPsAAAAUAEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAALAAAArQAAAJCAP////sAAAAiAEMAbwBuAHQAZQB4AHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAsAAABLAAAAAAAAAAA+wAAADIASQBtAGEAZwBlAFAAcgBvAHAAZQByAHQAaQBlAHMARABvAGMAawBXAGkAZABnAGUAdAEAAAM0AAAAuwAAAAAAAAAA+wAAABQARABvAGMAawBXAGkAZABnAGUAdAAAAAAsAAABvwAAAAAAAAAA+wAAADwATQBlAHMAaABQAHIAbwBwAGUAcgB0AGkAZQBzAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAALAAABDgAAACIAP////sAAABGAFIAZQBnAGkAcwB0AHIAYQB0AGkAbwBuAEgAaQBzAHQAbwByAHkAVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAA/////wAAAGAA////+wAAAC4AQgByAG8AdwBzAGUAcgBXAGkAZABnAGUAdABEAG8AYwBrAFcAaQBkAGcAZQB0AAAAAAD/////AAAApgD////7AAAAQABUAHIAYQBuAHMAZgBlAHIARgB1AG4AYwB0AGkAbwBuAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAAAP////8AAAF7AP////sAAAA+AFMAaABpAGYAdABDAG8AcgByAGUAYwB0AGkAbwBuAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAAAP////8AAAAyAP////sAAABCAEkAbQBhAGcAZQBSAGUAZwBpAHMAdAByAGEAdABpAG8AbgBXAGkAZABnAGUAdABEAG8AYwBrAFcAaQBkAGcAZQB0AQAAABoAAAOJAAABNwD////7AAAARgBQAGEAdABpAGUAbgB0AFIAZQBnAGkAcwB0AHIAYQB0AGkAbwBuAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAAAP////8AAAF/AP////sAAAA0AE4AYQB2AGkAZwBhAHQAaQBvAG4AVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAA/////wAAAOsA////AAAAAgAABpAAAACw/AEAAAAD+wAAADwAVABvAG8AbABQAHIAbwBwAGUAcgB0AGkAZQBzAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAAAAAAAXYAAAF2AP////sAAAA6AFAAbwBpAG4AdABTAGEAbQBwAGwAaQBuAGcAVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAAAAAC2wAAASYA////+wAAAD4ASQBtAGEAZwBlAFAAcgBvAHAAZQByAHQAaQBlAHMAVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAAAAAGkAAAAIsA////AAAFDgAAA4kAAAAEAAAABAAAAAgAAAAI/AAAAAEAAAACAAAABQAAABYARABhAHQAYQBUAG8AbwBsAEIAYQByAAAAAAD/////AAAAAAAAAAAAAAAWAFQAbwBvAGwAVABvAG8AbABCAGEAcgAAAAA3/////wAAAAAAAAAAAAAAIgBOAGEAdgBpAGcAYQB0AGkAbwBuAFQAbwBvAGwAQgBhAHIAAAAAAP////8AAAAAAAAAAAAAAB4AVwBvAHIAawBmAGwAbwB3AFQAbwBvAGwAQgBhAHIBAAAAAP////8AAAAAAAAAAAAAABwARABlAHMAawB0AG8AcABUAG8AbwBsAEIAYQByAQAAAvb/////AAAAAAAAAAA=");
    this->addDefaultDesktops("PatientRegistrationUid", "LAYOUT_3DACS_2X2", "AAAA/wAAAAD9AAAAAgAAAAAAAAGBAAADifwCAAAADPsAAAAUAEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAALAAAArQAAAJCAP////sAAAAiAEMAbwBuAHQAZQB4AHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAsAAABLAAAAAAAAAAA+wAAADIASQBtAGEAZwBlAFAAcgBvAHAAZQByAHQAaQBlAHMARABvAGMAawBXAGkAZABnAGUAdAEAAAM0AAAAuwAAAAAAAAAA+wAAABQARABvAGMAawBXAGkAZABnAGUAdAAAAAAsAAABvwAAAAAAAAAA+wAAADwATQBlAHMAaABQAHIAbwBwAGUAcgB0AGkAZQBzAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAALAAABDgAAACIAP////sAAABGAFIAZQBnAGkAcwB0AHIAYQB0AGkAbwBuAEgAaQBzAHQAbwByAHkAVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAA/////wAAAGAA////+wAAAC4AQgByAG8AdwBzAGUAcgBXAGkAZABnAGUAdABEAG8AYwBrAFcAaQBkAGcAZQB0AAAAAAD/////AAAApgD////7AAAAQABUAHIAYQBuAHMAZgBlAHIARgB1AG4AYwB0AGkAbwBuAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAAAP////8AAAF7AP////sAAAA+AFMAaABpAGYAdABDAG8AcgByAGUAYwB0AGkAbwBuAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAAAP////8AAAAyAP////sAAABCAEkAbQBhAGcAZQBSAGUAZwBpAHMAdAByAGEAdABpAG8AbgBXAGkAZABnAGUAdABEAG8AYwBrAFcAaQBkAGcAZQB0AAAAABoAAAOJAAABNwD////7AAAARgBQAGEAdABpAGUAbgB0AFIAZQBnAGkAcwB0AHIAYQB0AGkAbwBuAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQBAAAAGgAAA4kAAAF/AP////sAAAA0AE4AYQB2AGkAZwBhAHQAaQBvAG4AVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAA/////wAAAOsA////AAAAAgAABpAAAACw/AEAAAAD+wAAADwAVABvAG8AbABQAHIAbwBwAGUAcgB0AGkAZQBzAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAAAAAAAXYAAAF2AP////sAAAA6AFAAbwBpAG4AdABTAGEAbQBwAGwAaQBuAGcAVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAAAAAC2wAAASYA////+wAAAD4ASQBtAGEAZwBlAFAAcgBvAHAAZQByAHQAaQBlAHMAVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAAAAAGkAAAAIsA////AAAFDgAAA4kAAAAEAAAABAAAAAgAAAAI/AAAAAEAAAACAAAABQAAABYARABhAHQAYQBUAG8AbwBsAEIAYQByAAAAAAD/////AAAAAAAAAAAAAAAWAFQAbwBvAGwAVABvAG8AbABCAGEAcgAAAAA3/////wAAAAAAAAAAAAAAIgBOAGEAdgBpAGcAYQB0AGkAbwBuAFQAbwBvAGwAQgBhAHIAAAAAAP////8AAAAAAAAAAAAAAB4AVwBvAHIAawBmAGwAbwB3AFQAbwBvAGwAQgBhAHIBAAAAAP////8AAAAAAAAAAAAAABwARABlAHMAawB0AG8AcABUAG8AbwBsAEIAYQByAQAAAvb/////AAAAAAAAAAA=");
    this->addDefaultDesktops("PreOpPlanningUid", "LAYOUT_3DACS_2X2", "AAAA/wAAAAD9AAAAAgAAAAAAAAGBAAADifwCAAAADPsAAAAUAEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAALAAAAqYAAAJCAP////sAAAAiAEMAbwBuAHQAZQB4AHQARABvAGMAawBXAGkAZABnAGUAdAEAAAMgAAAAWgAAAAAAAAAA+wAAADIASQBtAGEAZwBlAFAAcgBvAHAAZQByAHQAaQBlAHMARABvAGMAawBXAGkAZABnAGUAdAEAAAM0AAAAuwAAAAAAAAAA+wAAABQARABvAGMAawBXAGkAZABnAGUAdAEAAAN7AAAAYAAAAAAAAAAA+wAAADwATQBlAHMAaABQAHIAbwBwAGUAcgB0AGkAZQBzAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAALAAAAW0AAACIAP////sAAABGAFIAZQBnAGkAcwB0AHIAYQB0AGkAbwBuAEgAaQBzAHQAbwByAHkAVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAA/////wAAAGAA////+wAAAC4AQgByAG8AdwBzAGUAcgBXAGkAZABnAGUAdABEAG8AYwBrAFcAaQBkAGcAZQB0AQAAABoAAAOJAAAApgD////7AAAAQABUAHIAYQBuAHMAZgBlAHIARgB1AG4AYwB0AGkAbwBuAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAAAP////8AAAF7AP////sAAAA+AFMAaABpAGYAdABDAG8AcgByAGUAYwB0AGkAbwBuAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAAAP////8AAAAyAP////sAAABCAEkAbQBhAGcAZQBSAGUAZwBpAHMAdAByAGEAdABpAG8AbgBXAGkAZABnAGUAdABEAG8AYwBrAFcAaQBkAGcAZQB0AAAAACwAAAS0AAABNwD////7AAAARgBQAGEAdABpAGUAbgB0AFIAZQBnAGkAcwB0AHIAYQB0AGkAbwBuAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAAAP////8AAAF/AP////sAAAA0AE4AYQB2AGkAZwBhAHQAaQBvAG4AVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAA/////wAAAOsA////AAAAAgAABpAAAACw/AEAAAAD+wAAADwAVABvAG8AbABQAHIAbwBwAGUAcgB0AGkAZQBzAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAAAAAAAXYAAAF2AP////sAAAA6AFAAbwBpAG4AdABTAGEAbQBwAGwAaQBuAGcAVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAAAAAC2wAAASYA////+wAAAD4ASQBtAGEAZwBlAFAAcgBvAHAAZQByAHQAaQBlAHMAVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAAAAAGkAAAAIsA////AAAFDgAAA4kAAAAEAAAABAAAAAgAAAAI/AAAAAEAAAACAAAABQAAABYARABhAHQAYQBUAG8AbwBsAEIAYQByAAAAAAD/////AAAAAAAAAAAAAAAWAFQAbwBvAGwAVABvAG8AbABCAGEAcgAAAAAA/////wAAAAAAAAAAAAAAIgBOAGEAdgBpAGcAYQB0AGkAbwBuAFQAbwBvAGwAQgBhAHIAAAAAAP////8AAAAAAAAAAAAAAB4AVwBvAHIAawBmAGwAbwB3AFQAbwBvAGwAQgBhAHIBAAAAAP////8AAAAAAAAAAAAAABwARABlAHMAawB0AG8AcABUAG8AbwBsAEIAYQByAQAAAvb/////AAAAAAAAAAA=");
    this->addDefaultDesktops("IntraOpImagingUid", "LAYOUT_3DACS_2X2", "AAAA/wAAAAD9AAAAAgAAAAAAAAGBAAADifwCAAAADPsAAAAUAEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAALAAAAqYAAAJCAP////sAAAAiAEMAbwBuAHQAZQB4AHQARABvAGMAawBXAGkAZABnAGUAdAEAAAMgAAAAWgAAAAAAAAAA+wAAADIASQBtAGEAZwBlAFAAcgBvAHAAZQByAHQAaQBlAHMARABvAGMAawBXAGkAZABnAGUAdAEAAAM0AAAAuwAAAAAAAAAA+wAAABQARABvAGMAawBXAGkAZABnAGUAdAEAAAN7AAAAYAAAAAAAAAAA+wAAADwATQBlAHMAaABQAHIAbwBwAGUAcgB0AGkAZQBzAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAALAAAAW0AAACIAP////sAAABGAFIAZQBnAGkAcwB0AHIAYQB0AGkAbwBuAEgAaQBzAHQAbwByAHkAVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAA/////wAAAGAA////+wAAAC4AQgByAG8AdwBzAGUAcgBXAGkAZABnAGUAdABEAG8AYwBrAFcAaQBkAGcAZQB0AQAAABoAAAOJAAAApgD////7AAAAQABUAHIAYQBuAHMAZgBlAHIARgB1AG4AYwB0AGkAbwBuAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAAAP////8AAAF7AP////sAAAA+AFMAaABpAGYAdABDAG8AcgByAGUAYwB0AGkAbwBuAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAAAP////8AAAAyAP////sAAABCAEkAbQBhAGcAZQBSAGUAZwBpAHMAdAByAGEAdABpAG8AbgBXAGkAZABnAGUAdABEAG8AYwBrAFcAaQBkAGcAZQB0AAAAACwAAAS0AAABNwD////7AAAARgBQAGEAdABpAGUAbgB0AFIAZQBnAGkAcwB0AHIAYQB0AGkAbwBuAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAAAP////8AAAF/AP////sAAAA0AE4AYQB2AGkAZwBhAHQAaQBvAG4AVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAA/////wAAAOsA////AAAAAgAABpAAAACw/AEAAAAD+wAAADwAVABvAG8AbABQAHIAbwBwAGUAcgB0AGkAZQBzAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAAAAAAAXYAAAF2AP////sAAAA6AFAAbwBpAG4AdABTAGEAbQBwAGwAaQBuAGcAVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAAAAAC2wAAASYA////+wAAAD4ASQBtAGEAZwBlAFAAcgBvAHAAZQByAHQAaQBlAHMAVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAAAAAGkAAAAIsA////AAAFDgAAA4kAAAAEAAAABAAAAAgAAAAI/AAAAAEAAAACAAAABQAAABYARABhAHQAYQBUAG8AbwBsAEIAYQByAAAAAAD/////AAAAAAAAAAAAAAAWAFQAbwBvAGwAVABvAG8AbABCAGEAcgAAAAAA/////wAAAAAAAAAAAAAAIgBOAGEAdgBpAGcAYQB0AGkAbwBuAFQAbwBvAGwAQgBhAHIAAAAAAP////8AAAAAAAAAAAAAAB4AVwBvAHIAawBmAGwAbwB3AFQAbwBvAGwAQgBhAHIBAAAAAP////8AAAAAAAAAAAAAABwARABlAHMAawB0AG8AcABUAG8AbwBsAEIAYQByAQAAAvb/////AAAAAAAAAAA=");
    this->addDefaultDesktops("PostOpControllUid", "LAYOUT_3DACS_2X2", "AAAA/wAAAAD9AAAAAgAAAAAAAAGBAAADifwCAAAADPsAAAAUAEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAALAAAAqYAAAJCAP////sAAAAiAEMAbwBuAHQAZQB4AHQARABvAGMAawBXAGkAZABnAGUAdAEAAAMgAAAAWgAAAAAAAAAA+wAAADIASQBtAGEAZwBlAFAAcgBvAHAAZQByAHQAaQBlAHMARABvAGMAawBXAGkAZABnAGUAdAEAAAM0AAAAuwAAAAAAAAAA+wAAABQARABvAGMAawBXAGkAZABnAGUAdAEAAAN7AAAAYAAAAAAAAAAA+wAAADwATQBlAHMAaABQAHIAbwBwAGUAcgB0AGkAZQBzAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAALAAAAW0AAACIAP////sAAABGAFIAZQBnAGkAcwB0AHIAYQB0AGkAbwBuAEgAaQBzAHQAbwByAHkAVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAA/////wAAAGAA////+wAAAC4AQgByAG8AdwBzAGUAcgBXAGkAZABnAGUAdABEAG8AYwBrAFcAaQBkAGcAZQB0AQAAABoAAAOJAAAApgD////7AAAAQABUAHIAYQBuAHMAZgBlAHIARgB1AG4AYwB0AGkAbwBuAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAAAP////8AAAF7AP////sAAAA+AFMAaABpAGYAdABDAG8AcgByAGUAYwB0AGkAbwBuAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAAAP////8AAAAyAP////sAAABCAEkAbQBhAGcAZQBSAGUAZwBpAHMAdAByAGEAdABpAG8AbgBXAGkAZABnAGUAdABEAG8AYwBrAFcAaQBkAGcAZQB0AAAAACwAAAS0AAABNwD////7AAAARgBQAGEAdABpAGUAbgB0AFIAZQBnAGkAcwB0AHIAYQB0AGkAbwBuAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAAAP////8AAAF/AP////sAAAA0AE4AYQB2AGkAZwBhAHQAaQBvAG4AVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAA/////wAAAOsA////AAAAAgAABpAAAACw/AEAAAAD+wAAADwAVABvAG8AbABQAHIAbwBwAGUAcgB0AGkAZQBzAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAAAAAAAXYAAAF2AP////sAAAA6AFAAbwBpAG4AdABTAGEAbQBwAGwAaQBuAGcAVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAAAAAC2wAAASYA////+wAAAD4ASQBtAGEAZwBlAFAAcgBvAHAAZQByAHQAaQBlAHMAVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAAAAAGkAAAAIsA////AAAFDgAAA4kAAAAEAAAABAAAAAgAAAAI/AAAAAEAAAACAAAABQAAABYARABhAHQAYQBUAG8AbwBsAEIAYQByAAAAAAD/////AAAAAAAAAAAAAAAWAFQAbwBvAGwAVABvAG8AbABCAGEAcgAAAAAA/////wAAAAAAAAAAAAAAIgBOAGEAdgBpAGcAYQB0AGkAbwBuAFQAbwBvAGwAQgBhAHIAAAAAAP////8AAAAAAAAAAAAAAB4AVwBvAHIAawBmAGwAbwB3AFQAbwBvAGwAQgBhAHIBAAAAAP////8AAAAAAAAAAAAAABwARABlAHMAawB0AG8AcABUAG8AbwBsAEIAYQByAQAAAvb/////AAAAAAAAAAA=");
    this->addDefaultDesktops("DEFAULT", "LAYOUT_3DACS_2X2", "AAAA/wAAAAD9AAAAAgAAAAAAAAGBAAAEOPwCAAAADPsAAAAUAEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAALAAAArQAAAJCAP////sAAAAiAEMAbwBuAHQAZQB4AHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAsAAABLAAAAAAAAAAA+wAAADIASQBtAGEAZwBlAFAAcgBvAHAAZQByAHQAaQBlAHMARABvAGMAawBXAGkAZABnAGUAdAEAAAM0AAAAuwAAAAAAAAAA+wAAABQARABvAGMAawBXAGkAZABnAGUAdAAAAAAsAAABvwAAAAAAAAAA+wAAADwATQBlAHMAaABQAHIAbwBwAGUAcgB0AGkAZQBzAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAALAAABDgAAACIAP////sAAABGAFIAZQBnAGkAcwB0AHIAYQB0AGkAbwBuAEgAaQBzAHQAbwByAHkAVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAA/////wAAAGAA////+wAAAC4AQgByAG8AdwBzAGUAcgBXAGkAZABnAGUAdABEAG8AYwBrAFcAaQBkAGcAZQB0AAAAAAD/////AAAApgD////7AAAAQABUAHIAYQBuAHMAZgBlAHIARgB1AG4AYwB0AGkAbwBuAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAAAP////8AAAF7AP////sAAAA+AFMAaABpAGYAdABDAG8AcgByAGUAYwB0AGkAbwBuAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAAAP////8AAAAyAP////sAAABCAEkAbQBhAGcAZQBSAGUAZwBpAHMAdAByAGEAdABpAG8AbgBXAGkAZABnAGUAdABEAG8AYwBrAFcAaQBkAGcAZQB0AAAAAAD/////AAABNwD////7AAAARgBQAGEAdABpAGUAbgB0AFIAZQBnAGkAcwB0AHIAYQB0AGkAbwBuAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAAAP////8AAAF/AP////sAAAA0AE4AYQB2AGkAZwBhAHQAaQBvAG4AVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAA/////wAAAOsA////AAAAAgAABpAAAACw/AEAAAAD+wAAADwAVABvAG8AbABQAHIAbwBwAGUAcgB0AGkAZQBzAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAAAAAAAXYAAAF2AP////sAAAA6AFAAbwBpAG4AdABTAGEAbQBwAGwAaQBuAGcAVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAAAAAC2wAAASYA////+wAAAD4ASQBtAGEAZwBlAFAAcgBvAHAAZQByAHQAaQBlAHMAVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAAAAAGkAAAAIsA////AAAGkAAAA4kAAAAEAAAABAAAAAgAAAAI/AAAAAEAAAACAAAABQAAABYARABhAHQAYQBUAG8AbwBsAEIAYQByAAAAAAD/////AAAAAAAAAAAAAAAWAFQAbwBvAGwAVABvAG8AbABCAGEAcgEAAAAA/////wAAAAAAAAAAAAAAIgBOAGEAdgBpAGcAYQB0AGkAbwBuAFQAbwBvAGwAQgBhAHIAAAAAAP////8AAAAAAAAAAAAAAB4AVwBvAHIAawBmAGwAbwB3AFQAbwBvAGwAQgBhAHIBAAAAav////8AAAAAAAAAAAAAABwARABlAHMAawB0AG8AcABUAG8AbwBsAEIAYQByAQAAA2D/////AAAAAAAAAAA=");
  };

  ~ApplicationsParser(){};

  void addDefaultDesktops(QString workflowStateUid, QString layoutUid, QString mainwindowstate)
  {
    mWorkflowDefaultDesktops[workflowStateUid] = Desktop(layoutUid,QByteArray::fromBase64(mainwindowstate.toAscii()));
  }

  Desktop getDefaultDesktop(QString applicationName, QString workflowName)
  {
    //TODO use applicationName!!!
    if(!mWorkflowDefaultDesktops.count(workflowName))
      return mWorkflowDefaultDesktops["DEFAULT"];
    return mWorkflowDefaultDesktops[workflowName];
  }

  Desktop getDesktop(QString applicationName, QString workflowName)
  {
    Desktop retval;
    QDomElement workflowElement = mXmlFile.descend(applicationName).descend("workflows").descend(workflowName).getElement();
    QDomElement desktopElement;
    if(workflowElement.namedItem("custom").isNull())
    {
      //desktopElement = workflowElement.namedItem("default").toElement();
      return this->getDefaultDesktop(applicationName, workflowName);
    }
    else
      desktopElement = workflowElement.namedItem("custom").toElement();
    retval.mMainWindowState = QByteArray::fromBase64(desktopElement.attribute("mainwindowstate").toAscii());
    retval.mLayoutUid = desktopElement.attribute("layoutuid");

    return retval;
  };

  void setDesktop(QString applicationName, QString workflowName, Desktop desktop)
  {
    QDomElement desktopElement = mXmlFile.descend(applicationName).descend("workflows").descend(workflowName).descend("custom").getElement();
    desktopElement.setAttribute("mainwindowstate", QString(desktop.mMainWindowState.toBase64()));
    desktopElement.setAttribute("layoutuid", desktop.mLayoutUid);
    mXmlFile.save();
  };

  void resetDesktop(QString applicationName, QString workflowName)
  {
    QDomElement workflowElement = mXmlFile.descend(applicationName).descend("workflows").descend(workflowName).getElement();
    workflowElement.removeChild(workflowElement.namedItem("custom"));
    mXmlFile.save();
  };

private:
  ssc::XmlOptionFile mXmlFile;
  std::map<QString, Desktop> mWorkflowDefaultDesktops;
};

StateManager *StateManager::mTheInstance = NULL;
StateManager* stateManager() { return StateManager::getInstance(); }
StateManager* StateManager::getInstance()
{
  if(mTheInstance == NULL)
   {
     mTheInstance = new StateManager();
     mTheInstance->initialize();
   }
   return mTheInstance;
}

void StateManager::destroyInstance()
{
    delete mTheInstance;
    mTheInstance = NULL;
}

StateManager::StateManager()
{

}

StateManager::~StateManager()
{}

WorkflowStateMachinePtr StateManager::getWorkflow()
{
  return mWorkflowStateMachine;
}

void StateManager::initialize()
{
  mPatientData.reset(new PatientData());

  mWorkflowStateMachine.reset(new WorkflowStateMachine());
  mWorkflowStateMachine->start();
}

PatientDataPtr StateManager::getPatientData()
{
  return mPatientData;
}

Desktop StateManager::getActiveDesktop()
{
  //TODO
  //which application?

  ApplicationsParser parser;
  //std::cout << "Getting desktop for state: " << mWorkflowStateMachine->getActiveUidState().toStdString() << std::endl;
  return parser.getDesktop("Lab", mWorkflowStateMachine->getActiveUidState());
}

void StateManager::saveDesktop(Desktop desktop)
{
  //TODO
  //which application?
  ApplicationsParser parser;
  parser.setDesktop("Lab", mWorkflowStateMachine->getActiveUidState(),desktop);
}

void StateManager::resetDesktop()
{
  //TODO
  //which application?
  ApplicationsParser parser;
  parser.resetDesktop("Lab", mWorkflowStateMachine->getActiveUidState());
}

void StateManager::addXml(QDomNode& dataNode)
{}

void StateManager::parseXml(QDomNode& dataNode)
{}

} //namespace cx
