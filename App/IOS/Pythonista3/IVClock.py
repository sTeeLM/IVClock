import ui
import cb
import datetime
import struct
import sys
import functools
import queue
import time
import PickerView


class IVClockBLE (object):
    def __init__(self, scan_notify, write_notify, update_notify, connect_notify, disconnect_notify):
        self.peripheral = None
        self.service = None
        self.characteristics = None
        self.scan_notify = scan_notify
        self.write_notify = write_notify
        self.update_notify = update_notify
        self.connect_notify = connect_notify
        self.disconnect_notify = disconnect_notify

    def did_discover_peripheral(self, p):
        print('Scanning:', p.name)
        self.scan_notify(p)
        if p.name and 'JDY-32-LE' in p.name and not self.peripheral:
            self.peripheral = p
            print('Connecting to ivclock...')
            cb.connect_peripheral(p)

    def did_connect_peripheral(self, p):
        print('Connected:', p.name)
        print('Discovering services...')
        p.discover_services()

    def did_fail_to_connect_peripheral(self, p, error):
        print('Failed to connect: %s' % (error,))

    def did_disconnect_peripheral(self, p, error):
        print('Disconnected, error: %s' % (error,))
        self.disconnect_notify(p, error)
        self.peripheral = None
        self.service    = None
        self.characteristics    = None

    def did_discover_services(self, p, error):
        for s in p.services:
            print('Discovered services:', s.uuid)
            if s.uuid == 'FFE0':
                print('Discovered ble service, discovering characteristitcs...')
                self.service = s
                p.discover_characteristics(s)

    def did_discover_characteristics(self, s, error):
        print('Did discover characteristics...')
        for c in s.characteristics:
            print('Discovered characteristics:', c.uuid)
            if c.uuid == 'FFE1':
                self.characteristics = c
                self.peripheral.set_notify_value(c, True)
                self.connect_notify(self.peripheral, c, error)

    def did_write_value(self, c, error):
        self.write_notify(self.peripheral, c, error)

    def did_update_value(self, c, error):
        self.update_notify(self.peripheral, c, error)

    def write_cmd(self, data):
        if self.peripheral and self.characteristics:
            self.peripheral.write_characteristic_value(self.characteristics, data, True)

class IVClock(object):
    CMDS = {
        'REMOTE_CONTROL_CMD_PING'      : [1, 0, ''],
        'REMOTE_CONTROL_CMD_GET_TIME'  : [2, 7, '<H5B'],
        'REMOTE_CONTROL_CMD_SET_TIME'  : [3, 7, '<H5B'],
        'REMOTE_CONTROL_CMD_GET_ALARM' : [4, 5, '<5B'],
        'REMOTE_CONTROL_CMD_SET_ALARM' : [5, 5, '<5B'],
        'REMOTE_CONTROL_CMD_GET_PARAM' : [6, 19,'<19B'],
        'REMOTE_CONTROL_CMD_SET_PARAM' : [7, 19,'<19B'],
        'REMOTE_CONTROL_CMD_STOP_ALARM' : [8, 0, ''],
        'REMOTE_CONTROL_CMD_GET_INFO'  : [9, 0, '']
    }
    RES = {
        'REMOTE_CONTROL_RES_PING'      : [101, 0, ''],
        'REMOTE_CONTROL_RES_GET_TIME'  : [102, 7, '<H5B'],
        'REMOTE_CONTROL_RES_SET_TIME'  : [103, 7, '<H5B'],
        'REMOTE_CONTROL_RES_GET_ALARM' : [104, 5, '<5B'],
        'REMOTE_CONTROL_RES_SET_ALARM' : [105, 5, '<5B'],
        'REMOTE_CONTROL_RES_GET_PARAM' : [106, 19,'<19B'],
        'REMOTE_CONTROL_RES_SET_PARAM' : [107, 19,'<19B'],
        'REMOTE_CONTROL_RES_STOP_ALARM' : [108, 0, ''],
        'REMOTE_CONTROL_RES_GET_INFO'  : [109, 31, '<fBff2B16s']
    }

    # event: {'ev' : event, 'enable_ui': True, 'call_back': cb}
    event_queue = queue.Queue()

    REMOTE_CONTROL_MSG_HEADER_MAGIC = 0x12345678
    MSG_HEADER_LENGTH = 8
    MSG_BODY_LENGTH   = 31
    MSG_LENGTH = MSG_HEADER_LENGTH + MSG_BODY_LENGTH # header + body
    view  = None
    ble = None
    actions = {}
    body_datetime = {}
    body_alarm = {}
    body_param = {}
    body_info  = {}
    msg_res = b''
    is_syncing_remote = False

    def __init__(self):
        self.actions = {
            'segTimeFormat' : self.segTimeFormat_action,
            'segTempFormat' : self.segTempFormat_action,
            'switchBP'      : self.switchBP_action,
            'switchLM'      : self.switchLM_action,
            'switchMM'      : self.switchMM_action,
            'segPS'         : self.segPS_action,
            'switchBS'      : self.switchBS_action,
            'pickerTmrSnd'  : self.pickerTmrSnd_action,
            'btnSyncTime'   : self.btnSyncTime_action,
            'btnStopAlarm'  : self.btnStopAlarm_action,
            'pickerAlarmIndex' : self.pickerAlarmIndex_action,
            'pickerAlarmSnd'   : self.pickerAlarmSnd_action,
            'switchAlarmDay1': self.switchAlarmDay_action,
            'switchAlarmDay2': self.switchAlarmDay_action,
            'switchAlarmDay3': self.switchAlarmDay_action,
            'switchAlarmDay4': self.switchAlarmDay_action,
            'switchAlarmDay5': self.switchAlarmDay_action,
            'switchAlarmDay6': self.switchAlarmDay_action,
            'switchAlarmDay0': self.switchAlarmDay_action,
            'sliderPlyVol'   : self.sliderPlyVol_action,
            'pickerBSFromTo': self.pickerBSFromTo_action,
            'pickerAlarmTime' : self.pickerAlarmTime_action,

        }
        
        self.body_datetime = {
            'year' : 0,
            'mon' : 0,
            'date' : 0,
            'hour' : 0,
            'min' : 0,
            'sec' : 0,
        }
        self.body_alarm = {
            'alarm_index' : 0,
            'day_mask' : 0,
            'hour' : 0,
            'min' : 0,
            'snd' : 0,
        }
        self.body_param = {
            'time_12' : 0,
            'acc_en' : 0,
            'bp_en' : 0,
            'alm1_en' : 0,
            'alm1_begin' : 0,
            'alm1_end' : 0,
            'mon_lt_en' : 0,
            'temp_cen' : 0,
            'power_timeo' : 0,
            'tmr_snd' : 0,
            'ply_vol' : 0,
            'min_ply_vol' : 0,
            'max_ply_vol' : 0,
            'min_power_timeo' : 0,
            'max_power_timeo' : 0,
            'step_power_timeo' : 0,
            'tmr_snd_cnt' : 0,
            'alarm_cnt' : 0,
            'alarm_snd_cnt' : 0,
        }
        self.body_info = {
            'bat_voltage' : 0,
            'bat_quantity' : 0,
            'temp_cen' : 0,
            'temp_fah' : 0,
            'firmware_version_major' : 0,
            'firmware_version_minor' : 0,
            'build_id' : b'',
        }

    def init_ui(self):
        self.view = ui.load_view('IVClock')
        
        
        for view in self.view.subviews[0].subviews:
            if view.name in self.actions:
                view.action = self.actions[view.name]
            if view.name == 'pickerBSFromTo':
                view.component = [[str(x) + '点' for x in range(0, 24)],
                [str(x) + '点' for x in range(0, 24)]]
            elif view.name == 'pickerTmrSnd':
                view.component = [['音效' + str(x) for x in range(0, 10)]]
            elif view.name == 'pickerAlarmIndex':
                view.component = [['闹钟' + str(x) for x in range(0, 10)]]
            elif view.name == 'pickerAlarmSnd':
                view.component = [['音乐' + str(x) for x in range(0, 10)]]
            elif view.name == 'pickerAlarmTime':
                view.component = [[str(x) + '点' for x in range(0, 24)],
                [str(x) + '分' for x in range(0, 60)]]
        self.enableControls(False)
        self.view.present(style='full_screen', hide_title_bar=True)


    def init_ble(self):
        self.ble = IVClockBLE(
            self.scan_notify,
            self.write_notify, 
            self.update_notify, 
            self.connect_notify, 
            self.disconnect_notify)

    def syncRemote(self):
        time.sleep(1)
        self.is_syncing_remote = True
        self.sendCmd('REMOTE_CONTROL_CMD_GET_TIME')

    def getControlByName(self, name):
        for view in self.view.subviews[0].subviews:
            if view.name == name:
                return view
        return None

    def enableControls(self, enable):
        for view in self.view.subviews[0].subviews:
            if view.name == 'pickerBSFromTo':
                view.enabled = enable and self.body_param['alm1_en']
            elif view.name == 'sliderPlyVol':
                view.touch_enabled = enable
            else:
                view.enabled = enable

    def callbackConnect(self, param):
        print('callbackConnect', param)
        cb.stop_scan()
        self.syncRemote()

    def callbackDisconnect(self, param):
        print('callbackDisconnect', param)
        self.enableControls(False)
        cb.reset();
        cb.set_central_delegate(self.ble)
        cb.scan_for_peripherals();

    def callbackPing(self, param):
        print('callbackPing', param)
        pass

    def callbackDatetime(self, param):
        print('callbackDatetime', param)
        print(self.body_datetime)
        self.getControlByName('labelInfoDatetime').text = '%04d-%02d-%02d %02d:%02d:%02d' % (
            self.body_datetime['year'],
            self.body_datetime['mon'],
            self.body_datetime['date'],
            self.body_datetime['hour'],
            self.body_datetime['min'],
            self.body_datetime['sec'],
        )

        if param['sync_remote']:
            self.sendCmd('REMOTE_CONTROL_CMD_GET_PARAM')
        else:
            self.enableControls(True)

    def callbackParam(self, param):
        print('callbackParam', param)
        print(self.body_param)
        if param['sync_remote']:
            self.getControlByName('segPS').segments = ['禁止'] + ['%d秒'%x for x in range(self.body_param['min_power_timeo'], 
                self.body_param['max_power_timeo'] + 1, self.body_param['step_power_timeo'])]
            self.getControlByName('pickerAlarmIndex').component = [['闹钟' + str(x) for x in range(0, self.body_param['alarm_cnt'])]]
            self.getControlByName('pickerTmrSnd').component = [['音效' + str(x) for x in range(0, 10)]]
            self.getControlByName('pickerAlarmSnd').component = [['音乐' + str(x) for x in range(0, self.body_param['alarm_snd_cnt'])]]


        self.getControlByName('segTimeFormat').selected_index = 0 if self.body_param['time_12'] else 1
        self.getControlByName('segTempFormat').selected_index = 0 if self.body_param['temp_cen'] else 1
        self.getControlByName('switchBP').value = True if self.body_param['bp_en'] else False
        self.getControlByName('switchLM').value = True if self.body_param['mon_lt_en'] else False
        self.getControlByName('switchMM').value = True if self.body_param['acc_en'] else False
        self.getControlByName('segPS').selected_index = int(self.body_param['power_timeo'] / self.body_param['step_power_timeo'])
        self.getControlByName('switchBS').value = True if self.body_param['alm1_en'] else False
        if self.getControlByName('switchBS').value:
            self.getControlByName('pickerBSFromTo').value = [self.body_param['alm1_begin'],
                self.body_param['alm1_end']]
        self.getControlByName('pickerTmrSnd').value = [self.body_param['tmr_snd']]
        self.getControlByName('sliderPlyVol').value = float(self.body_param['ply_vol'] - self.body_param['min_ply_vol']) / float(self.body_param['max_ply_vol'] - self.body_param['min_ply_vol'])
        print(self.getControlByName('sliderPlyVol').value)
        
        if param['sync_remote']:
            self.sendCmd('REMOTE_CONTROL_CMD_GET_INFO')
        else:
            self.enableControls(True)        

    def callbackInfo(self, param):
        print('callbackInfo', param)
        print(self.body_info)
        self.getControlByName('labelInfoBat').text = '%.2f V  %03d%%' % (self.body_info['bat_voltage'], self.body_info['bat_quantity'])
        self.getControlByName('labelInfoTemp').text = '%.2f' % (self.body_info['temp_cen'])
        self.getControlByName('labelInfoBuildID').text = functools.reduce(lambda x,y: '%s%02x'%(x,y), self.body_info['build_id'])
        self.getControlByName('labelInfoVersion').text = '%02d.%02d' % (self.body_info['firmware_version_major'], self.body_info['firmware_version_minor'])

        if param['sync_remote']:
            self.sendCmd('REMOTE_CONTROL_CMD_GET_ALARM')
        else:
            self.enableControls(True) 

    def callbackAlarm(self, param):
        print('callbackAlarm', param)
        print(self.body_alarm)
        self.getControlByName('pickerAlarmIndex').value = [self.body_alarm['alarm_index']]
        self.getControlByName('pickerAlarmTime').value = [self.body_alarm['hour'], self.body_alarm['min']]
        self.getControlByName('pickerAlarmSnd').value = [self.body_alarm['snd']]
        self.getControlByName('switchAlarmDay0').value = True if self.body_alarm['day_mask'] & 0x1 else False
        self.getControlByName('switchAlarmDay1').value = True if self.body_alarm['day_mask'] & 0x2 else False
        self.getControlByName('switchAlarmDay2').value = True if self.body_alarm['day_mask'] & 0x4 else False
        self.getControlByName('switchAlarmDay3').value = True if self.body_alarm['day_mask'] & 0x8 else False
        self.getControlByName('switchAlarmDay4').value = True if self.body_alarm['day_mask'] & 0x10 else False
        self.getControlByName('switchAlarmDay5').value = True if self.body_alarm['day_mask'] & 0x20 else False
        self.getControlByName('switchAlarmDay6').value = True if self.body_alarm['day_mask'] & 0x40  else False

        if param['sync_remote']:
            self.is_syncing_remote = False
        self.enableControls(True)
        
    def callbackStopAlarm(self, param):
        self.enableControls(True)

    def sendCmd(self, cmd):
        print('sendCmd', cmd)
        header_magic  = self.REMOTE_CONTROL_MSG_HEADER_MAGIC
        header_body_length = self.CMDS[cmd][1]
        header_cmd_res = self.CMDS[cmd][0]
        header_code = 0
        msg_header = struct.pack('<IHBB', header_magic, header_body_length, header_cmd_res, header_code)
        msg_body = b''
        if cmd == 'REMOTE_CONTROL_CMD_PING' :
            pass
        elif cmd == 'REMOTE_CONTROL_CMD_GET_TIME' or cmd == 'REMOTE_CONTROL_CMD_SET_TIME':
            msg_body = struct.pack(self.CMDS[cmd][2], 
                self.body_datetime['year'],
                self.body_datetime['mon'],
                self.body_datetime['date'],
                self.body_datetime['hour'],
                self.body_datetime['min'],
                self.body_datetime['sec'])
        elif cmd == 'REMOTE_CONTROL_CMD_GET_ALARM' or cmd == 'REMOTE_CONTROL_CMD_SET_ALARM':
            msg_body = struct.pack(self.CMDS[cmd][2], 
                self.body_alarm['alarm_index'],
                self.body_alarm['day_mask'],
                self.body_alarm['hour'],
                self.body_alarm['min'],
                self.body_alarm['snd'])
        elif cmd == 'REMOTE_CONTROL_CMD_GET_PARAM' or cmd == 'REMOTE_CONTROL_CMD_SET_PARAM':
            msg_body = struct.pack(self.CMDS[cmd][2], 
                self.body_param['time_12'],
                self.body_param['acc_en'],
                self.body_param['bp_en'],
                self.body_param['alm1_en'],
                self.body_param['alm1_begin'],
                self.body_param['alm1_end'],
                self.body_param['mon_lt_en'],
                self.body_param['temp_cen'],
                self.body_param['power_timeo'],
                self.body_param['tmr_snd'],
                self.body_param['ply_vol'],
                self.body_param['min_ply_vol'],
                self.body_param['max_ply_vol'],
                self.body_param['min_power_timeo'],
                self.body_param['max_power_timeo'],
                self.body_param['step_power_timeo'],
                self.body_param['tmr_snd_cnt'],
                self.body_param['alarm_cnt'],
                self.body_param['alarm_snd_cnt'])
        elif cmd == 'REMOTE_CONTROL_CMD_STOP_ALARM':
            pass
        elif cmd == 'REMOTE_CONTROL_CMD_GET_INFO':
            pass
        
        msg = msg_header + msg_body;
        if(len(msg) < self.MSG_LENGTH):
            msg = msg + b'\0' * (self.MSG_LENGTH - len(msg))
        self.ble.write_cmd(msg)

    def lookupRes(self, res_code):
        for res_str in self.RES:
            if self.RES[res_str][0] == res_code:
                return res_str
        return None

    def recvRes(self, res_msg):
        res = ''
        (header_magic, header_body_length, header_cmd_res, header_ret_code) = struct.unpack('<IHBB', res_msg[0:self.MSG_HEADER_LENGTH])
        print('recvRes', res_msg.hex())
        print('header_magic       %x'%(header_magic))
        print('header_body_length %d'%(header_body_length))
        print('header_cmd_res     %d'%(header_cmd_res))
        print('header_ret_code    %d'%(header_ret_code))
        res = self.lookupRes(header_cmd_res)
        if header_magic != self.REMOTE_CONTROL_MSG_HEADER_MAGIC or header_body_length > self.MSG_BODY_LENGTH or header_ret_code != 0 or not res:
            print('INVALID Message!')
            return
        if res == 'REMOTE_CONTROL_RES_PING':
            self.event_queue.put({'ev' : 'UPDATE_PING', 'param':{'sync_remote': self.is_syncing_remote}, 'callback': self.callbackPing})
        elif res == 'REMOTE_CONTROL_RES_GET_TIME' or res == 'REMOTE_CONTROL_RES_SET_TIME' :
            (self.body_datetime['year'],
            self.body_datetime['mon'],
            self.body_datetime['date'],
            self.body_datetime['hour'],
            self.body_datetime['min'],
            self.body_datetime['sec']) = struct.unpack(self.RES[res][2], res_msg[self.MSG_HEADER_LENGTH:self.MSG_HEADER_LENGTH + self.RES[res][1]])
            self.event_queue.put({'ev' : 'UPDATE_DATETIME', 'param':{'sync_remote': self.is_syncing_remote}, 'callback': self.callbackDatetime})
        elif res == 'REMOTE_CONTROL_RES_GET_ALARM' or res == 'REMOTE_CONTROL_RES_SET_ALARM':
            (self.body_alarm['alarm_index'],
            self.body_alarm['day_mask'],
            self.body_alarm['hour'],
            self.body_alarm['min'],
            self.body_alarm['snd']) = struct.unpack(self.RES[res][2], res_msg[self.MSG_HEADER_LENGTH:self.MSG_HEADER_LENGTH + self.RES[res][1]])
            self.event_queue.put({'ev' : 'UPDATE_ALARM', 'param':{'sync_remote': self.is_syncing_remote}, 'callback': self.callbackAlarm})
        elif res == 'REMOTE_CONTROL_RES_GET_PARAM' or res == 'REMOTE_CONTROL_RES_SET_PARAM':
            (self.body_param['time_12'],
            self.body_param['acc_en'],
            self.body_param['bp_en'],
            self.body_param['alm1_en'],
            self.body_param['alm1_begin'],
            self.body_param['alm1_end'],
            self.body_param['mon_lt_en'],
            self.body_param['temp_cen'],
            self.body_param['power_timeo'],
            self.body_param['tmr_snd'],
            self.body_param['ply_vol'],
            self.body_param['min_ply_vol'],
            self.body_param['max_ply_vol'],
            self.body_param['min_power_timeo'],
            self.body_param['max_power_timeo'],
            self.body_param['step_power_timeo'],
            self.body_param['tmr_snd_cnt'],
            self.body_param['alarm_cnt'],
            self.body_param['alarm_snd_cnt']) = struct.unpack(self.RES[res][2], res_msg[self.MSG_HEADER_LENGTH:self.MSG_HEADER_LENGTH + self.RES[res][1]])
            self.event_queue.put({'ev' : 'UPDATE_PARAM', 'param': {'sync_remote': self.is_syncing_remote}, 'callback': self.callbackParam})
        elif res == 'REMOTE_CONTROL_RES_STOP_ALARM':
            self.event_queue.put({'ev' : 'STOP_ALARM', 'param': {'sync_remote': self.is_syncing_remote}, 'callback': self.callbackStopAlarm})
        elif res == 'REMOTE_CONTROL_RES_GET_INFO': 
            (self.body_info['bat_voltage'],
            self.body_info['bat_quantity'],
            self.body_info['temp_cen'],
            self.body_info['temp_fah'],
            self.body_info['firmware_version_major'],
            self.body_info['firmware_version_minor'],
            self.body_info['build_id']) = struct.unpack(self.RES[res][2], res_msg[self.MSG_HEADER_LENGTH:self.MSG_HEADER_LENGTH + self.RES[res][1]])
            self.event_queue.put({'ev' : 'UPDATE_INFO', 'param': {'sync_remote': self.is_syncing_remote}, 'callback': self.callbackInfo})
        
    def scan_notify(self, p):
        if not self.view.on_screen:
            cb.reset()

    def update_notify(self, p, c, error):
        print('update_notify', c.value.hex())
        if c.value : 
            self.msg_res += c.value;
            if(len(self.msg_res) >= self.MSG_LENGTH):
                saved_msg = self.msg_res
                self.msg_res = b''
                self.recvRes(saved_msg)    

    def write_notify(self, p, c, error):
        print('write_notify', c.value)

    def connect_notify(self, p, c, error):
        print('connect_notify', c.value)
        self.event_queue.put({'ev':'CONNECT', 'param':{}, 'callback':self.callbackConnect})

    def disconnect_notify(self, p, error):
        print('disconnect_notify', p)
        self.event_queue.put({'ev':'DISCONNECT', 'param':{}, 'callback':self.callbackDisconnect})

    def btnSyncTime_action(self, sender):
        self.enableControls(False)
        current_datetime = datetime.datetime.now()
        self.body_datetime['year'] = current_datetime.year
        self.body_datetime['mon'] = current_datetime.month
        self.body_datetime['date'] = current_datetime.day
        self.body_datetime['hour'] = current_datetime.hour
        self.body_datetime['min'] = current_datetime.minute
        self.body_datetime['sec'] = current_datetime.second
        self.sendCmd('REMOTE_CONTROL_CMD_SET_TIME')

    def btnStopAlarm_action(self, sender):
        self.enableControls(False)
        self.sendCmd('REMOTE_CONTROL_CMD_STOP_ALARM')
        pass

    def segTimeFormat_action(self, sender):
        self.enableControls(False)
        self.body_param['time_12'] = 1 if sender.selected_index == 0 else 0
        self.sendCmd('REMOTE_CONTROL_CMD_SET_PARAM')

    def segTempFormat_action(self, sender):
        self.enableControls(False)
        self.body_param['temp_cen'] = 1 if sender.selected_index == 0 else 0
        self.sendCmd('REMOTE_CONTROL_CMD_SET_PARAM')

    def switchBP_action(self, sender):
        self.enableControls(False)
        self.body_param['bp_en'] = 1 if sender.value else 0
        self.sendCmd('REMOTE_CONTROL_CMD_SET_PARAM')

    def switchLM_action(self, sender):
        self.enableControls(False)
        self.body_param['mon_lt_en'] = 1 if sender.value else 0
        self.sendCmd('REMOTE_CONTROL_CMD_SET_PARAM')

    def switchMM_action(self, sender):
        self.enableControls(False)
        self.body_param['acc_en'] = 1 if sender.value else 0
        self.sendCmd('REMOTE_CONTROL_CMD_SET_PARAM')

    def segPS_action(self, sender):
        self.enableControls(False)
        self.body_param['power_timeo'] = sender.selected_index * self.body_param['step_power_timeo']
        if self.body_param['power_timeo'] > self.body_param['max_power_timeo']:
            self.body_param['power_timeo'] = self.body_param['max_power_timeo']
        self.sendCmd('REMOTE_CONTROL_CMD_SET_PARAM')

    def switchBS_action(self, sender):
        self.enableControls(False)
        self.body_param['alm1_en'] = 1 if sender.value else 0
        self.sendCmd('REMOTE_CONTROL_CMD_SET_PARAM')

    def pickerBSFromTo_action(self, sender):
        self.enableControls(False)
        self.body_param['alm1_begin'] = sender.value[0]
        self.body_param['alm1_end'] = sender.value[1]
        self.sendCmd('REMOTE_CONTROL_CMD_SET_PARAM')

    def pickerTmrSnd_action(self, sender):
        self.enableControls(False)
        self.body_param['tmr_snd'] = sender.value[0]
        self.sendCmd('REMOTE_CONTROL_CMD_SET_PARAM')

    def sliderPlyVol_action(self, sender):
        self.enableControls(False)
        self.body_param['ply_vol'] = self.body_param['min_ply_vol'] + int(sender.value * (self.body_param['max_ply_vol'] - self.body_param['min_ply_vol']))
        self.sendCmd('REMOTE_CONTROL_CMD_SET_PARAM')  

    def pickerAlarmIndex_action(self, sender):
        self.enableControls(False)
        self.body_alarm['alarm_index'] = sender.value[0]
        self.sendCmd('REMOTE_CONTROL_CMD_GET_ALARM') 

    def pickerAlarmTime_action(self, sender):
        self.enableControls(False)
        self.body_alarm['hour'] = sender.value[0]
        self.body_alarm['min'] = sender.value[1]
        self.sendCmd('REMOTE_CONTROL_CMD_SET_ALARM') 

    def pickerAlarmSnd_action(self, sender):
        self.enableControls(False)
        self.body_alarm['snd'] = sender.value[0]
        self.sendCmd('REMOTE_CONTROL_CMD_SET_ALARM') 

    def switchAlarmDay_action(self, sender):
        self.enableControls(False)
        alarm_index = int(sender.name[-1])
        value = self.body_alarm['day_mask']
        print(alarm_index, sender.value, value)
        if not sender.value:
            val = 1 << alarm_index
            val ^= 0xFF
            value &= (val & 0xFF)
        else:
            value |= ((1 << alarm_index) & 0x7F)
        self.body_alarm['day_mask'] = value
        print(self.body_alarm)
        self.sendCmd('REMOTE_CONTROL_CMD_SET_ALARM') 

    def eventLoop(self):
        event = None
        while self.view.on_screen:
            try:
                event = self.event_queue.get(block=True, timeout=1)
            except queue.Empty:
                event = None
                pass
            if event:
                print('get event', event)
                event['callback'](event['param'])
        print('Quit!')

def main(argv):
    iv = IVClock()
    iv.init_ui()
    iv.init_ble()
    cb.set_central_delegate(iv.ble)
    print('Scanning for peripherals...')
    cb.scan_for_peripherals()
    iv.eventLoop()

main(sys.argv)
