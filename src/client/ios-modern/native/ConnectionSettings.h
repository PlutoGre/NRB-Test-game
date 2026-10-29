#pragma once
#include <string>

// The selection is fixed before the game creates network workers.
bool OPRemoteEnabled();
std::string OPRemoteBaseURL();
std::string OPRemoteInvite();
std::string OPResourceURL(const std::string& original);

#ifdef __OBJC__
#import <UIKit/UIKit.h>
UIViewController *OPConnectionController(void (^completion)(void));
void OPAuthorizeResourceRequest(NSMutableURLRequest *request);
#endif
