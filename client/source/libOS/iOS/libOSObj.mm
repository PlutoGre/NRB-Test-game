
#include "libOS.h"
#import "libOSObj.h"



@implementation libOSObj

- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{
    if (alertView) {
        NSString* str = nil;
        
        UITextField* txtf = [alertView viewWithTag:1001];
        if (txtf) {
            str = [txtf text];
        }
        else
        {
            UITextView* txtv = [alertView viewWithTag:1002];
            if(txtv)
            {
                str = [txtv text];
            }
            
        }
        
        static std::string outstr;
        if(str!=nil)
        {

            outstr = ([str UTF8String]);
            libOS::getInstance()->_boardcastInputBoxOK(outstr);
        }
        else
        {
            libOS::getInstance()->_boardcastMessageboxOK([alertView tag]);
        }
    }
}

-(void) showWait
{
    if(waitView)[waitView startAnimating];
}
-(void) hideWait;
{
    if(waitView)[waitView stopAnimating];
}

@end
