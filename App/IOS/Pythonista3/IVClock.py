import ui

def btnConnect_action(sender):
    sender.enabled = False
    pass

def btnSyncTime_action(sender):
    print(sender.enabled)
    pass

def segTimeFormat_action(sender):
    pass

def segTempFormat_action(sender):
    pass

def switchBP_action(sender):
    pass

def switchLM_action(sender):
    pass

def switchMM_action(sender):
    pass

def segPS_action(sender):
    pass

def switchBS_action(sender):
    pass

def datepickerBSFrom_action(sender):
    print("sender is %s val is %d:%d" % (sender.name,sender.date.hour, sender.date.minute))
    pass

def datepickerBSTo_action(sender):
    pass

def segBP_action(sender):
    pass

def segAlarmIndex_action(sender):
    pass

def datepickerAlarmTime_action(sender):
    pass

def segAlarmSnd_action(sender):
    print("sender is %s val is %s" % (sender.name, sender.selected_index))
    pass

def switchAlarmDay_action(sender):
    print("sender is %s" % (sender.name))
    pass

def sliderPlyVol_action(sender):
    pass

v = ui.load_view('IVClock')

if min(ui.get_screen_size()) >= 768:
	# iPad
	v.frame = (0, 0, 360, 400)
	v.present('sheet')
else:
	# iPhone
	v.present(orientations=['portrait'])
	
print(v.subviews[0].subviews)
