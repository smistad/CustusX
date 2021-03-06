/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/
#ifndef CXCUSTOMMETAIMAGE_H_
#define CXCUSTOMMETAIMAGE_H_

#include "cxResourceExport.h"

#include <QString>
#include "cxTransform3D.h"

namespace cx
{

typedef boost::shared_ptr<class CustomMetaImage> CustomMetaImagePtr;

/**\brief utility class for accessing metaheader files.
 *
 * This is meant as a supplement to vtkMetaImageReader/Writer,
 * extending that interface.
 *
 * \ingroup cx_resource_core_utilities
 */
class cxResource_EXPORT CustomMetaImage
{
public:
  static CustomMetaImagePtr create(QString filename) { return CustomMetaImagePtr(new CustomMetaImage(filename)); }
  explicit CustomMetaImage(QString filename);

  Transform3D readTransform();
  void setTransform(const Transform3D M);

  QString readModality();
  QString readImageType();
  void setModality(QString value);
  void setImageType(QString value);

  QString readKey(QString key);
  void setKey(QString key, QString value);

private:
  QString mFilename;

  void remove(QStringList* data, QStringList keys);
  void append(QStringList* data, QString key, QString value);

};

}

#endif /* CXCUSTOMMETAIMAGE_H_ */
