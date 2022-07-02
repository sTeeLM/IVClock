# coding: utf-8

# https://forum.omz-software.com/topic/4592/picker-wheel-for-lists-not-just-dates/2

from objc_util import ObjCInstance, c, ObjCClass, ns, create_objc_class, NSObject
from ctypes import c_void_p
import ui


# ObjC classes
UIColor = ObjCClass('UIColor')
UIPickerView = ObjCClass('UIPickerView')
UIFont = ObjCClass('UIFont')
NSAttributedString = ObjCClass('NSAttributedString')


# Default attributes, no need to recreate them again and again
def _str_symbol(name):
    return ObjCInstance(c_void_p.in_dll(c, name))
    
    
_default_attributes = {
    _str_symbol('NSFontAttributeName'): UIFont.fontWithName_size_(ns('Courier'), 16),
    _str_symbol('NSForegroundColorAttributeName'): UIColor.blackColor(),
    _str_symbol('NSBackgroundColorAttributeName'): UIColor.whiteColor()
}


# Data source & delegate methods
def pickerView_attributedTitleForRow_forComponent_(self, cmd, picker_view, row, component):
    inst = PickerView._all_instances[ObjCInstance(picker_view)]
    return NSAttributedString.alloc().initWithString_attributes_(ns(inst.component[component][row]), ns(_default_attributes)).ptr
    
    
def pickerView_titleForRow_forComponent_(self, cmd, picker_view, row, component):
    inst = PickerView._all_instances[ObjCInstance(picker_view)]
    return ns(inst.component[component][row]).ptr
    
    
def pickerView_numberOfRowsInComponent_(self, cmd, picker_view, component):
    inst = PickerView._all_instances[ObjCInstance(picker_view)]
    return len(inst.component[component])
    
    
def numberOfComponentsInPickerView_(self, cmd, picker_view):
    inst = PickerView._all_instances[ObjCInstance(picker_view)]
    return len(inst.component)
    
    
def rowSize_forComponent_(self, cmd, picker_view, component):
    return 100
    
    
def pickerView_rowHeightForComponent_(self, cmd, picker_view, component):
    return 30
   
    
def pickerView_didSelectRow_inComponent_(self, cmd, picker_view, row, component):
    inst = PickerView._all_instances[ObjCInstance(picker_view)]
    print('get instance', ObjCInstance(picker_view))
    if inst._action and callable(inst._action):
        inst._action(inst)
    
    
_methods = [
    numberOfComponentsInPickerView_, pickerView_numberOfRowsInComponent_,
    rowSize_forComponent_, pickerView_rowHeightForComponent_, pickerView_attributedTitleForRow_forComponent_,
    pickerView_didSelectRow_inComponent_
]

_protocols = ['UIPickerViewDataSource', 'UIPickerViewDelegate']




UIPickerViewDataSourceAndDelegate = create_objc_class(
    'UIPickerViewDataSourceAndDelegate', NSObject, methods=_methods, protocols=_protocols)

# UIPickerView wrapper which behaves like ui.View (in terms of init, layout, ...)
class PickerView(ui.View):
    _all_instances = {}
    _component = [[str(x) for x in range(0,10)]]
    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self._action = None
        self._picker_view = UIPickerView.alloc().initWithFrame_(ObjCInstance(self).bounds()).autorelease()
        ObjCInstance(self).addSubview_(self._picker_view)
        datasource = UIPickerViewDataSourceAndDelegate.alloc().init().autorelease()
        self._picker_view.setDelegate_(datasource)
        self._picker_view.setDataSource_(datasource)
        self.delegate_and_datasource = datasource
        self._all_instances[ObjCInstance(self._picker_view)] = self
        print('add instance', ObjCInstance(self._picker_view))

    def __del__(self):
        print('del instance', ObjCInstance(self._picker_view))
        del self._all_instances[ObjCInstance(self._picker_view)]

    def layout(self):
        self._picker_view.frame = ObjCInstance(self).bounds()
        
    @property
    def value(self):
        return [self._picker_view.selectedRowInComponent_(x) for x in range(0, len(self._component))]
        
    @value.setter
    def value(self, x):
        for i in range(0,len(x)) :
            self._picker_view.selectRow_inComponent_animated_(x[i], i, True)
        
    @property
    def delegate(self):
        return self._picker_view.delegate()
        
    @delegate.setter
    def delegate(self, x):
        self._picker_view.setDelegate_(x)
        
    @property
    def data_source(self):
        return self._picker_view.dataSource()
        
    @data_source.setter
    def data_source(self, x):
        self._picker_view.setDataSource_(x)
        
    @property
    def action(self):
        return self._action

    @action.setter
    def action(self, x):
        self._action = x

    @property
    def enabled(self):
        return self._picker_view.isEnabled()

    @enabled.setter
    def enabled(self, x): 
        self._picker_view.setEnabled(x)
        self._picker_view.alpha = 1 if x else 0.5 
 
    @property
    def component(self):
        return self._component
        
    @component.setter
    def component(self, x):
        self._component = x
    
class MyView(ui.View):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        
        self.background_color = 'white'
        
        pv1 = PickerView(frame=[100, 100, 200, 100])
        pv1.action = self.fuck
        self.add_subview(pv1)
        
def fuck(sender):
    print('action called', sender.value)
        

if __name__ == '__main__':
    v = ui.load_view('PickerView')
    print(v.subviews[0].frame)
    pv1 = PickerView(frame=v.subviews[0].frame)
    pv1.action = fuck
    pv1.component = [[str(x) for x in range(0,9)],
        [str(x) for x in range(12, 33)]]
    pv1.value = [6,2]
    pv1.enabled = False
    print(pv1.enabled)
    pv1.enabled = True
    print(pv1.enabled)
    v.add_subview(pv1)
    v.present('full_screen')
