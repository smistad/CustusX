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

#include "cxHelpBrowser.h"

#include <QHelpEngine>
#include "cxHelpEngine.h"
#include "cxTypeConversions.h"
#include <iostream>
#include <QDesktopServices>

namespace cx
{
HelpBrowser::HelpBrowser(QWidget *parent, HelpEnginePtr engine)
	: QTextBrowser(parent), mEngine(engine)
{
	connect(mEngine.get(), SIGNAL(keywordActivated(QString)), this, SLOT(showHelpForKeyword(const QString&)));
//	this->setOpenExternalLinks(true);
}

void HelpBrowser::showHelpForKeyword(const QString &id)
{
	if (mEngine->engine())
	{
		QMap<QString, QUrl> links = mEngine->engine()->linksForIdentifier(id);
//		std::cout << "[links]" << links.size() << std::endl;
		if (links.count())
		{
//			std::cout << "Set links: " << links.size()
//					  << ", " << links.first().toString()
//					  << ", " << links.firstKey()
//					  << std::endl;
//			std::cout << "sourceH: " << links.first().toString() << std::endl;
//			std::cout << "sourceE: " << QString(links.first().toEncoded()) << std::endl;

			setSource(links.first());
		}
	}
}

void HelpBrowser::setSource(const QUrl& name)
{
	if (name.scheme() == "qthelp")
		QTextBrowser::setSource(name);
	else
	{
		QDesktopServices::openUrl(name);
	}

}

QVariant HelpBrowser::loadResource(int type, const QUrl &name)
{
	if (type < 4 && mEngine->engine())
	{
		QUrl url(name);
		if (name.isRelative())
			url = source().resolved(url);

//		std::cout << "HelpBrowser::loadResource, scheme=" << url.scheme() << "  url=" << url.toString() << std::endl;

		if (url.scheme() == "qthelp")
			return QVariant(mEngine->engine()->fileData(url));
		else
			return QTextBrowser::loadResource(type, url);
	}
	return QVariant();
}

}//end namespace cx