/*
 *   test_main.cpp
 *
 *	 Copyright(c) 2012 by Joe Kilner
 *   This file is part of Panoptik.
 *
 *   Panoptik is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Panoptik is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public License
 *   along with Panoptik.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <iostream>
#include "OpenNI.h"

using namespace std;
using namespace xn;

int main()
{
	XnStatus rc;
	Context *context(0);

	EnumerationErrors errors;
	context = new Context;
	rc = context->Init();
	if (rc == XN_STATUS_NO_NODE_PRESENT)
	{
		XnChar strError[1024];
		errors.ToString(strError, 1024);
		printf("%s\n", strError);
		exit(1);
	}
	else if (rc != XN_STATUS_OK)
	{
		printf("Open failed: %s\n", xnGetStatusString(rc));
		exit(1);
	}
	NodeInfoList *depth_list(0);
	depth_list = new NodeInfoList;
	rc = context->EnumerateProductionTrees(XN_NODE_TYPE_DEPTH,NULL,*depth_list,NULL);
	if (rc != XN_STATUS_OK)
	{
		printf("Enumeration failed: %s\n", xnGetStatusString(rc));
		exit(1);
	}

	DepthGenerator *pGenerator = new DepthGenerator();

	int deviceID(0);
	int mode(2);

	printf("=== Intialising Depth: %d %d\n", deviceID, mode);
	int count = 0;
	for (NodeInfoList::Iterator i = depth_list->Begin();
			i != depth_list->End();
			++i, count++)
	{
		cout << count << " " << deviceID;
		if (count == deviceID)
		{
			NodeInfo ni = *i;
			cout << "ni = " << ni.GetCreationInfo();
			XnProductionNodeDescription desc = ni.GetDescription();
			printf("strVendor = %s, strName = %s, creationInfo = %s, name = %s\n",
					desc.strVendor, desc.strName, ni.GetCreationInfo(),
					ni.GetInstanceName());

			context->CreateProductionTree(ni, *pGenerator);

			unsigned mode_count = pGenerator->GetSupportedMapOutputModesCount ();
			printf("mode_count = %d\n", mode_count);

			XnMapOutputMode* modes = new XnMapOutputMode[mode_count];
			pGenerator->GetSupportedMapOutputModes (modes, mode_count);
			for (unsigned x = 0 ; x < mode_count; ++x) {
				printf("mode %d, FPS = %d, XRes = %d, YRes = %d\n",
				x,modes[x].nFPS, modes[x].nXRes, modes[x].nYRes);
			}

			pGenerator->SetMapOutputMode (modes[mode]);
			pGenerator->StartGenerating ();
		}
	}

	cout << "OK!" << endl;
	return 0;
}
