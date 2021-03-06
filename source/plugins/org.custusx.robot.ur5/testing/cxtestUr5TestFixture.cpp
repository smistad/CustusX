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

#include "cxtestUr5TestFixture.h"

#include <QFile>

namespace cxtest
{

Ur5TestFixture::Ur5TestFixture() :
    mUr5Robot(),
    mUr5Connection()
{
}

QByteArray Ur5TestFixture::getTestData(int packetSize)
{
    if(packetSize == 560)
    {
        QString filename = "C:/Dev/cx/Cx/CX/source/plugins/org.custusx.robot.ur5/testing/test.data/cxDataFromRobot560.txt";
        QFile file(filename);
        if (!file.open(QIODevice::ReadOnly)) return QByteArray();
        return QByteArray::fromHex(file.readAll());
    }
    else if(packetSize == 1254)
    {
        QString filename = "C:/Dev/cx/Cx/CX/source/plugins/org.custusx.robot.ur5/testing/test.data/cxDataFromRobot1254.txt";
        QFile file(filename);
        if (!file.open(QIODevice::ReadOnly)) return QByteArray();
        return QByteArray::fromHex(file.readAll());
    }
    else if(packetSize == 1460)
    {
        QString filename = "C:/Dev/cx/Cx/CX/source/plugins/org.custusx.robot.ur5/testing/test.data/cxDataFromRobot1460.txt";
        QFile file(filename);
        if (!file.open(QIODevice::ReadOnly)) return QByteArray();
        return QByteArray::fromHex(file.readAll());
    }
    else
    {
        return QByteArray(0);
    }
}

Eigen::MatrixXd Ur5TestFixture::jacobianUr5(Eigen::RowVectorXd jointPositions)
{
    Eigen::RowVectorXd s(6), c(6);

    s << sin(jointPositions(0)), sin(jointPositions(1)), sin(jointPositions(2)),
            sin(jointPositions(3)), sin(jointPositions(4)), sin(jointPositions(5));

    c << cos(jointPositions(0)), cos(jointPositions(1)), cos(jointPositions(2)),
            cos(jointPositions(3)), cos(jointPositions(4)), cos(jointPositions(5));


    Eigen::MatrixXd matrix(6,6);

    matrix << (2183*c(0))/20000 + (823*c(0)*c(4))/10000 + (17*c(1)*s(0))/40 - (1893*c(3)*(c(1)*s(0)*s(2) + c(2)*s(0)*s(1)))/20000 + (1893*s(3)*(s(0)*s(1)*s(2) - c(1)*c(2)*s(0)))/20000 - (823*s(4)*(c(3)*(s(0)*s(1)*s(2) - c(1)*c(2)*s(0)) + s(3)*(c(1)*s(0)*s(2) + c(2)*s(0)*s(1))))/10000 - (157*s(0)*s(1)*s(2))/400 + (157*c(1)*c(2)*s(0))/400,
               c(0)*((17*s(1))/40 + (157*c(1)*s(2))/400 + (157*c(2)*s(1))/400 + (823*s(4)*(c(3)*(c(1)*s(2) + c(2)*s(1)) + s(3)*(c(1)*c(2) - s(1)*s(2))))/10000 + (1893*c(3)*(c(1)*c(2) - s(1)*s(2)))/20000 - (1893*s(3)*(c(1)*s(2) + c(2)*s(1)))/20000),
               c(0)*((157*c(1)*s(2))/400 + (157*c(2)*s(1))/400 + (823*s(4)*(c(3)*(c(1)*s(2) + c(2)*s(1)) + s(3)*(c(1)*c(2) - s(1)*s(2))))/10000 + (1893*c(3)*(c(1)*c(2) - s(1)*s(2)))/20000 - (1893*s(3)*(c(1)*s(2) + c(2)*s(1)))/20000),
               c(0)*((823*s(4)*(c(3)*(c(1)*s(2) + c(2)*s(1)) + s(3)*(c(1)*c(2) - s(1)*s(2))))/10000 + (1893*c(3)*(c(1)*c(2) - s(1)*s(2)))/20000 - (1893*s(3)*(c(1)*s(2) + c(2)*s(1)))/20000),
               -(c(3)*(c(1)*c(2) - s(1)*s(2)) - s(3)*(c(1)*s(2) + c(2)*s(1)))*((823*c(0)*c(4))/10000 - (1893*c(3)*(c(1)*s(0)*s(2) + c(2)*s(0)*s(1)))/20000 + (1893*s(3)*(s(0)*s(1)*s(2) - c(1)*c(2)*s(0)))/20000 - (823*s(4)*(c(3)*(s(0)*s(1)*s(2) - c(1)*c(2)*s(0)) + s(3)*(c(1)*s(0)*s(2) + c(2)*s(0)*s(1))))/10000) - (c(3)*(c(1)*s(0)*s(2) + c(2)*s(0)*s(1)) - s(3)*(s(0)*s(1)*s(2) - c(1)*c(2)*s(0)))*((823*s(4)*(c(3)*(c(1)*s(2) + c(2)*s(1)) + s(3)*(c(1)*c(2) - s(1)*s(2))))/10000 + (1893*c(3)*(c(1)*c(2) - s(1)*s(2)))/20000 - (1893*s(3)*(c(1)*s(2) + c(2)*s(1)))/20000),
              (823*s(4)*(c(3)*(c(1)*s(2) + c(2)*s(1)) + s(3)*(c(1)*c(2) - s(1)*s(2)))*(c(0)*c(4) - s(4)*(c(3)*(s(0)*s(1)*s(2) - c(1)*c(2)*s(0)) + s(3)*(c(1)*s(0)*s(2) + c(2)*s(0)*s(1)))))/10000 - s(4)*(c(3)*(c(1)*s(2) + c(2)*s(1)) + s(3)*(c(1)*c(2) - s(1)*s(2)))*((823*c(0)*c(4))/10000 - (823*s(4)*(c(3)*(s(0)*s(1)*s(2) - c(1)*c(2)*s(0)) + s(3)*(c(1)*s(0)*s(2) + c(2)*s(0)*s(1))))/10000),
              (2183*s(0))/20000 - (17*c(0)*c(1))/40 + (823*c(4)*s(0))/10000 + (1893*c(3)*(c(0)*c(1)*s(2) + c(0)*c(2)*s(1)))/20000 - (1893*s(3)*(c(0)*s(1)*s(2) - c(0)*c(1)*c(2)))/20000 + (823*s(4)*(c(3)*(c(0)*s(1)*s(2) - c(0)*c(1)*c(2)) + s(3)*(c(0)*c(1)*s(2) + c(0)*c(2)*s(1))))/10000 + (157*c(0)*s(1)*s(2))/400 - (157*c(0)*c(1)*c(2))/400,
               s(0)*((17*s(1))/40 + (157*c(1)*s(2))/400 + (157*c(2)*s(1))/400 + (823*s(4)*(c(3)*(c(1)*s(2) + c(2)*s(1)) + s(3)*(c(1)*c(2) - s(1)*s(2))))/10000 + (1893*c(3)*(c(1)*c(2) - s(1)*s(2)))/20000 - (1893*s(3)*(c(1)*s(2) + c(2)*s(1)))/20000),
               s(0)*((157*c(1)*s(2))/400 + (157*c(2)*s(1))/400 + (823*s(4)*(c(3)*(c(1)*s(2) + c(2)*s(1)) + s(3)*(c(1)*c(2) - s(1)*s(2))))/10000 + (1893*c(3)*(c(1)*c(2) - s(1)*s(2)))/20000 - (1893*s(3)*(c(1)*s(2) + c(2)*s(1)))/20000),
               s(0)*((823*s(4)*(c(3)*(c(1)*s(2) + c(2)*s(1)) + s(3)*(c(1)*c(2) - s(1)*s(2))))/10000 + (1893*c(3)*(c(1)*c(2) - s(1)*s(2)))/20000 - (1893*s(3)*(c(1)*s(2) + c(2)*s(1)))/20000),
               (c(3)*(c(0)*c(1)*s(2) + c(0)*c(2)*s(1)) - s(3)*(c(0)*s(1)*s(2) - c(0)*c(1)*c(2)))*((823*s(4)*(c(3)*(c(1)*s(2) + c(2)*s(1)) + s(3)*(c(1)*c(2) - s(1)*s(2))))/10000 + (1893*c(3)*(c(1)*c(2) - s(1)*s(2)))/20000 - (1893*s(3)*(c(1)*s(2) + c(2)*s(1)))/20000) - (c(3)*(c(1)*c(2) - s(1)*s(2)) - s(3)*(c(1)*s(2) + c(2)*s(1)))*((823*c(4)*s(0))/10000 + (1893*c(3)*(c(0)*c(1)*s(2) + c(0)*c(2)*s(1)))/20000 - (1893*s(3)*(c(0)*s(1)*s(2) - c(0)*c(1)*c(2)))/20000 + (823*s(4)*(c(3)*(c(0)*s(1)*s(2) - c(0)*c(1)*c(2)) + s(3)*(c(0)*c(1)*s(2) + c(0)*c(2)*s(1))))/10000),
              (823*s(4)*(c(3)*(c(1)*s(2) + c(2)*s(1)) + s(3)*(c(1)*c(2) - s(1)*s(2)))*(c(4)*s(0) + s(4)*(c(3)*(c(0)*s(1)*s(2) - c(0)*c(1)*c(2)) + s(3)*(c(0)*c(1)*s(2) + c(0)*c(2)*s(1)))))/10000 - s(4)*(c(3)*(c(1)*s(2) + c(2)*s(1)) + s(3)*(c(1)*c(2) - s(1)*s(2)))*((823*c(4)*s(0))/10000 + (823*s(4)*(c(3)*(c(0)*s(1)*s(2) - c(0)*c(1)*c(2)) + s(3)*(c(0)*c(1)*s(2) + c(0)*c(2)*s(1))))/10000),
               0,
               c(0)*((2183*s(0))/20000 - (17*c(0)*c(1))/40 + (823*c(4)*s(0))/10000 + (1893*c(3)*(c(0)*c(1)*s(2) + c(0)*c(2)*s(1)))/20000 - (1893*s(3)*(c(0)*s(1)*s(2) - c(0)*c(1)*c(2)))/20000 + (823*s(4)*(c(3)*(c(0)*s(1)*s(2) - c(0)*c(1)*c(2)) + s(3)*(c(0)*c(1)*s(2) + c(0)*c(2)*s(1))))/10000 + (157*c(0)*s(1)*s(2))/400 - (157*c(0)*c(1)*c(2))/400) - s(0)*((2183*c(0))/20000 + (823*c(0)*c(4))/10000 + (17*c(1)*s(0))/40 - (1893*c(3)*(c(1)*s(0)*s(2) + c(2)*s(0)*s(1)))/20000 + (1893*s(3)*(s(0)*s(1)*s(2) - c(1)*c(2)*s(0)))/20000 - (823*s(4)*(c(3)*(s(0)*s(1)*s(2) - c(1)*c(2)*s(0)) + s(3)*(c(1)*s(0)*s(2) + c(2)*s(0)*s(1))))/10000 - (157*s(0)*s(1)*s(2))/400 + (157*c(1)*c(2)*s(0))/400), c(0)*((2183*s(0))/20000 + (823*c(4)*s(0))/10000 + (1893*c(3)*(c(0)*c(1)*s(2) + c(0)*c(2)*s(1)))/20000 - (1893*s(3)*(c(0)*s(1)*s(2) - c(0)*c(1)*c(2)))/20000 + (823*s(4)*(c(3)*(c(0)*s(1)*s(2) - c(0)*c(1)*c(2)) + s(3)*(c(0)*c(1)*s(2) + c(0)*c(2)*s(1))))/10000 + (157*c(0)*s(1)*s(2))/400 - (157*c(0)*c(1)*c(2))/400) - s(0)*((2183*c(0))/20000 + (823*c(0)*c(4))/10000 - (1893*c(3)*(c(1)*s(0)*s(2) + c(2)*s(0)*s(1)))/20000 + (1893*s(3)*(s(0)*s(1)*s(2) - c(1)*c(2)*s(0)))/20000 - (823*s(4)*(c(3)*(s(0)*s(1)*s(2) - c(1)*c(2)*s(0)) + s(3)*(c(1)*s(0)*s(2) + c(2)*s(0)*s(1))))/10000 - (157*s(0)*s(1)*s(2))/400 + (157*c(1)*c(2)*s(0))/400), c(0)*((2183*s(0))/20000 + (823*c(4)*s(0))/10000 + (1893*c(3)*(c(0)*c(1)*s(2) + c(0)*c(2)*s(1)))/20000 - (1893*s(3)*(c(0)*s(1)*s(2) - c(0)*c(1)*c(2)))/20000 + (823*s(4)*(c(3)*(c(0)*s(1)*s(2) - c(0)*c(1)*c(2)) + s(3)*(c(0)*c(1)*s(2) + c(0)*c(2)*s(1))))/10000) - s(0)*((2183*c(0))/20000 + (823*c(0)*c(4))/10000 - (1893*c(3)*(c(1)*s(0)*s(2) + c(2)*s(0)*s(1)))/20000 + (1893*s(3)*(s(0)*s(1)*s(2) - c(1)*c(2)*s(0)))/20000 - (823*s(4)*(c(3)*(s(0)*s(1)*s(2) - c(1)*c(2)*s(0)) + s(3)*(c(1)*s(0)*s(2) + c(2)*s(0)*s(1))))/10000),
              -(c(3)*(c(1)*s(0)*s(2) + c(2)*s(0)*s(1)) - s(3)*(s(0)*s(1)*s(2) - c(1)*c(2)*s(0)))*((823*c(4)*s(0))/10000 + (1893*c(3)*(c(0)*c(1)*s(2) + c(0)*c(2)*s(1)))/20000 - (1893*s(3)*(c(0)*s(1)*s(2) - c(0)*c(1)*c(2)))/20000 + (823*s(4)*(c(3)*(c(0)*s(1)*s(2) - c(0)*c(1)*c(2)) + s(3)*(c(0)*c(1)*s(2) + c(0)*c(2)*s(1))))/10000) - (c(3)*(c(0)*c(1)*s(2) + c(0)*c(2)*s(1)) - s(3)*(c(0)*s(1)*s(2) - c(0)*c(1)*c(2)))*((823*c(0)*c(4))/10000 - (1893*c(3)*(c(1)*s(0)*s(2) + c(2)*s(0)*s(1)))/20000 + (1893*s(3)*(s(0)*s(1)*s(2) - c(1)*c(2)*s(0)))/20000 - (823*s(4)*(c(3)*(s(0)*s(1)*s(2) - c(1)*c(2)*s(0)) + s(3)*(c(1)*s(0)*s(2) + c(2)*s(0)*s(1))))/10000), ((823*c(4)*s(0))/10000 + (823*s(4)*(c(3)*(c(0)*s(1)*s(2) - c(0)*c(1)*c(2)) + s(3)*(c(0)*c(1)*s(2) + c(0)*c(2)*s(1))))/10000)*(c(0)*c(4) - s(4)*(c(3)*(s(0)*s(1)*s(2) - c(1)*c(2)*s(0)) + s(3)*(c(1)*s(0)*s(2) + c(2)*s(0)*s(1)))) - (c(4)*s(0) + s(4)*(c(3)*(c(0)*s(1)*s(2) - c(0)*c(1)*c(2)) + s(3)*(c(0)*c(1)*s(2) + c(0)*c(2)*s(1))))*((823*c(0)*c(4))/10000 - (823*s(4)*(c(3)*(s(0)*s(1)*s(2) - c(1)*c(2)*s(0)) + s(3)*(c(1)*s(0)*s(2) + c(2)*s(0)*s(1))))/10000),
               0,                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          s(0),                                                                                                                                                                                                                                                                                                                                                                                                                                                                           s(0),s(0),
               c(3)*(c(0)*c(1)*s(2) + c(0)*c(2)*s(1)) - s(3)*(c(0)*s(1)*s(2) - c(0)*c(1)*c(2)),
               c(4)*s(0) + s(4)*(c(3)*(c(0)*s(1)*s(2) - c(0)*c(1)*c(2)) + s(3)*(c(0)*c(1)*s(2) + c(0)*c(2)*s(1))),

              0,                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         -c(0),
              -c(0),                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     -c(0),
              c(3)*(c(1)*s(0)*s(2) + c(2)*s(0)*s(1)) - s(3)*(s(0)*s(1)*s(2) - c(1)*c(2)*s(0)),
              s(4)*(c(3)*(s(0)*s(1)*s(2) - c(1)*c(2)*s(0)) + s(3)*(c(1)*s(0)*s(2) + c(2)*s(0)*s(1))) - c(0)*c(4),

              1,                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 0,
              0,                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             0,                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               s(3)*(c(1)*s(2) + c(2)*s(1)) - c(3)*(c(1)*c(2) - s(1)*s(2)),
              -s(4)*(c(3)*(c(1)*s(2) + c(2)*s(1)) + s(3)*(c(1)*c(2) - s(1)*s(2)));

    return matrix;
}

} //cxtest
