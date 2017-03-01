﻿Enable Logon Auditing to Track Logon Activities of Windows Users:
To enable Logon Auditing, we need to configure Windows Group Policy settings. Press “Win + R”, type gpedit.msc and press the Enter button to open Windows Group Policy Editor.
Once you are in the Group Policy Editor, navigate to “Computer Configuration -> Windows Settings -> Security Settings -> Local Policies” and then select “Audit Policy” in the left pane.
The above action will show you some policies on the right pane. Here double click on “Audit logon events” policy to open it. Please don’t confuse “Audit logon events” with “Audit account logon events” as it is a setting for a completely different purpose.
Once the Window is opened, select both the check boxes “Success” and “Failure.” Now click on the “Apply” and “Ok” buttons to save the changes.