//
//  Debugger.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 19.05.18.
//

import Foundation

//
// Panel independent controls
//

extension MyController {
    
    @IBAction func stepIntoAction(_ sender: Any!) {
        
        document?.updateChangeCount(.changeDone)
        c64.step()
        refresh()
    }
  
    @IBAction func stepOverAction(_ sender: Any!) {

        document?.updateChangeCount(.changeDone)
        
        // If the next instruction is a JSR instruction, ...
        if (c64.cpu.readPC() == 0x20) {

            // we set soft breakpoint at next command
            c64.cpu.setSoftBreakpoint(c64.cpu.addressOfNextInstruction())
            c64.run()

        } else {
            
            // Same as step
            stepIntoAction(self)
        }
    }
    
    @IBAction func stopAndGoAction(_ sender: Any!) {
    
        document?.updateChangeCount(.changeDone)
        if c64.isHalted() {
            c64.run()
        } else {
            c64.halt()
            debugPanel.open()
        }
        refresh()
    }
    
    @IBAction func pauseAction(_ sender: Any!) {
        
        if c64.isRunning() {
            c64.halt()
            debugPanel.open()
        }
        refresh()
    }
    
    @IBAction func continueAction(_ sender: Any!) {
        
        document?.updateChangeCount(.changeDone)
        if c64.isHalted() {
            c64.run()
        }
        refresh()
    }
    
    @IBAction func setDecimalAction(_ sender: Any!) {
  
        hex = false
        cpuTableView.setHex(false)

        let bF = MyFormatter.init(inFormat: "[0-9]{0,3}", outFormat: "%03d", hex: false)
        let sF = MyFormatter.init(inFormat: "[0-9]{0,3}", outFormat: "%03d", hex: false)
        let wF = MyFormatter.init(inFormat: "[0-9]{0,5}", outFormat: "%05d", hex: false)
        refresh(bF, word: wF, threedigit: sF)
    }
    
    @IBAction func setHexadecimalAction(_ sender: Any!) {
        
        hex = true
        cpuTableView.setHex(true)

        let bF = MyFormatter.init(inFormat: "[0-9,a-f,A-F]{0,2}", outFormat: "%02X", hex: true)
        let sF = MyFormatter.init(inFormat: "[0-9,a-f,A-F]{0,3}", outFormat: "%03X", hex: true)
        let wF = MyFormatter.init(inFormat: "[0-9,a-f,A-F]{0,4}", outFormat: "%04X", hex: true)
        refresh(bF, word: wF, threedigit: sF)
    }
}

//
// CPU debug panel
//

extension MyController {
 
    // Registers
    
    func _pcAction(_ value: UInt16) {
        
        let undoValue = c64.cpu.pc()
        undoManager?.registerUndo(withTarget: self) {
            me in me._pcAction(undoValue)
        }
        undoManager?.setActionName("Set Program Counter")
        
        c64.cpu.setPC(value)
        refresh()
    }
    
    @IBAction func pcAction(_ sender: Any!) {
        
        let sender = sender as! NSTextField
        let value = UInt16(sender.intValue)
        if (value != c64.cpu.pc()) {
            _pcAction(value)
        }
    }
    
    func _aAction(_ value: UInt8) {
        
        let undoValue = c64.cpu.a()
        undoManager?.registerUndo(withTarget: self) {
            me in me._aAction(undoValue)
        }
        undoManager?.setActionName("Set Accumulator")
        
        c64.cpu.setA(value)
        refresh()
    }
    
    @IBAction func aAction(_ sender: Any!) {
        
        let sender = sender as! NSTextField
        let value = UInt8(sender.intValue)
        if (value != c64.cpu.a()) {
            _aAction(value)
        }
    }
    
    func _xAction(_ value: UInt8) {
        
        let undoValue = c64.cpu.x()
        undoManager?.registerUndo(withTarget: self) {
            me in me._xAction(undoValue)
        }
        undoManager?.setActionName("Set X Register")
        
        c64.cpu.setX(value)
        refresh()
    }
    
    @IBAction func xAction(_ sender: Any!) {
        
        let sender = sender as! NSTextField
        let value = UInt8(sender.intValue)
        if (value != c64.cpu.x()) {
            _xAction(value)
        }
    }

    func _yAction(_ value: UInt8) {
        
        let undoValue = c64.cpu.y()
        undoManager?.registerUndo(withTarget: self) {
            me in me._yAction(undoValue)
        }
        undoManager?.setActionName("Set Y Register")
        
        c64.cpu.setY(value)
        refresh()
    }
    
    @IBAction func yAction(_ sender: Any!) {
        
        let sender = sender as! NSTextField
        let value = UInt8(sender.intValue)
        if (value != c64.cpu.y()) {
            _yAction(value)
        }
    }
    
    func _spAction(_ value: UInt8) {
        
        let undoValue = c64.cpu.sp()
        undoManager?.registerUndo(withTarget: self) {
            me in me._spAction(undoValue)
        }
        undoManager?.setActionName("Set Stack Pointer")
        
        c64.cpu.setSP(value)
        refresh()
    }
    
    @IBAction func spAction(_ sender: Any!) {
        
        let sender = sender as! NSTextField
        let value = UInt8(sender.intValue)
        if (value != c64.cpu.sp()) {
            _spAction(value)
        }
    }
    
    // Processor flags
    
    func _nAction(_ value: Bool) {
        
        let undoValue = c64.cpu.nflag()
        undoManager?.registerUndo(withTarget: self) {
            me in me._nAction(undoValue)
        }
        undoManager?.setActionName("Toggle Negative Flag")
        
        c64.cpu.setNflag(value)
        refresh()
    }
    
    @IBAction func NAction(_ sender: Any!) {
        
        let sender = sender as! NSButton
        _nAction(sender.intValue != 0)
    }

    func _zAction(_ value: Bool) {
        
        let undoValue = c64.cpu.zflag()
        undoManager?.registerUndo(withTarget: self) {
            me in me._zAction(undoValue)
        }
        undoManager?.setActionName("Toggle Zero Flag")
        
        c64.cpu.setZflag(value)
        refresh()
    }
    
    @IBAction func ZAction(_ sender: Any!) {
        
        let sender = sender as! NSButton
        _zAction(sender.intValue != 0)
    }

    func _cAction(_ value: Bool) {
        
        let undoValue = c64.cpu.cflag()
        undoManager?.registerUndo(withTarget: self) {
            me in me._cAction(undoValue)
        }
        undoManager?.setActionName("Toggle Carry Flag")
        
        c64.cpu.setCflag(value)
        refresh()
    }
    
    @IBAction func CAction(_ sender: Any!) {
        
        let sender = sender as! NSButton
        _cAction(sender.intValue != 0)
    }
    
    func _iAction(_ value: Bool) {
        
        let undoValue = c64.cpu.iflag()
        undoManager?.registerUndo(withTarget: self) {
            me in me._iAction(undoValue)
        }
        undoManager?.setActionName("Toggle Interrupt Flag")
        
        c64.cpu.setIflag(value)
        refresh()
    }
    
    @IBAction func IAction(_ sender: Any!) {
        
        let sender = sender as! NSButton
        _iAction(sender.intValue != 0)
    }
    
    func _bAction(_ value: Bool) {
        
        let undoValue = c64.cpu.bflag()
        undoManager?.registerUndo(withTarget: self) {
            me in me._bAction(undoValue)
        }
        undoManager?.setActionName("Toggle Break Flag")
        
        c64.cpu.setBflag(value)
        refresh()
    }
    
    @IBAction func BAction(_ sender: Any!) {
        
        let sender = sender as! NSButton
        _bAction(sender.intValue != 0)
    }
    
    func _dAction(_ value: Bool) {
        
        let undoValue = c64.cpu.dflag()
        undoManager?.registerUndo(withTarget: self) {
            me in me._dAction(undoValue)
        }
        undoManager?.setActionName("Toggle Decimal Flag")
        
        c64.cpu.setDflag(value)
        refresh()
    }
    
    @IBAction func DAction(_ sender: Any!) {
        
        let sender = sender as! NSButton
        _dAction(sender.intValue != 0)
    }
    
    func _vAction(_ value: Bool) {
        
        let undoValue = c64.cpu.vflag()
        undoManager?.registerUndo(withTarget: self) {
            me in me._vAction(undoValue)
        }
        undoManager?.setActionName("Toggle Overflow Flag")
        
        c64.cpu.setVflag(value)
        refresh()
    }
    
    @IBAction func VAction(_ sender: Any!) {
        
        let sender = sender as! NSButton
        _vAction(sender.intValue != 0)
    }
    
    func _setBreakpointAction(_ value: UInt16) {
        
        undoManager?.registerUndo(withTarget: self) {
            me in me._deleteBreakpointAction(value)
        }
        undoManager?.setActionName("Set Breakpoint")
        
        c64.cpu.setHardBreakpoint(value)
        cpuTableView.reloadData()
    }
 
    func _deleteBreakpointAction(_ value: UInt16) {
        
        undoManager?.registerUndo(withTarget: self) {
            me in me._setBreakpointAction(value)
        }
        undoManager?.setActionName("Set Breakpoint")
        
        c64.cpu.deleteHardBreakpoint(value)
        cpuTableView.reloadData()
    }
    
    @IBAction func breakpointAction(_ sender: Any!) {
        
        let sender = sender as! NSTextField
        let value = UInt16(sender.intValue)
        if !c64.cpu.hardBreakpoint(value) {
            _setBreakpointAction(value)
        }
    }
    
    @objc func refreshCPU() {

        pc.intValue = Int32(c64.cpu.pc())
        a.intValue = Int32(c64.cpu.a())
        x.intValue = Int32(c64.cpu.x())
        y.intValue = Int32(c64.cpu.y())
        sp.intValue = Int32(c64.cpu.sp())
        nflag.intValue = c64.cpu.nflag() ? 1 : 0
        vflag.intValue = c64.cpu.vflag() ? 1 : 0
        bflag.intValue = c64.cpu.bflag() ? 1 : 0
        dflag.intValue = c64.cpu.dflag() ? 1 : 0
        iflag.intValue = c64.cpu.iflag() ? 1 : 0
        zflag.intValue = c64.cpu.zflag() ? 1 : 0
        cflag.intValue = c64.cpu.cflag() ? 1 : 0
    }
}

//
// Memory debug panel
//

extension MyController {

    @IBAction func setMemSource(_ sender: Any!) {
        
        let sender = sender as! NSPopUpButton
        memTableView.setMemView(sender.selectedTag())
    }
    
    @IBAction func setHighlighting(_ sender: Any!) {
        
        let sender = sender as! NSPopUpButton
        memTableView.setHighlighting(sender.selectedTag())
    }
}

//
// CIA debug panel
//

extension MyController {
    
    @objc func refreshCIA() {
        
        var info: CIAInfo
        
        if ciaSelector.indexOfSelectedItem == 0 {
                info = c64.cia1.getInfo()
                ciaIntLineLow.title = "IRQ line active"
        } else {
                info = c64.cia2.getInfo()
                ciaIntLineLow.title = "NMI line active"
        }

        ciaPA.intValue = Int32(info.portA.port)
        ciaPAbinary.intValue = Int32(info.portA.port)
        ciaPRA.intValue = Int32(info.portA.reg)
        ciaDDRA.intValue = Int32(info.portA.dir)

        ciaPB.intValue = Int32(info.portB.port)
        ciaPBbinary.intValue = Int32(info.portB.port)
        ciaPRB.intValue = Int32(info.portB.reg)
        ciaDDRB.intValue = Int32(info.portB.dir)

        ciaTimerA.intValue = Int32(info.timerA.count)
        ciaLatchA.intValue = Int32(info.timerA.latch)
        ciaRunningA.state = info.timerA.running ? .on : .off
        ciaToggleA.state = info.timerA.toggle ? .on : .off
        ciaPBoutA.state = info.timerA.pbout ? .on : .off
        ciaOneShotA.state = info.timerA.oneShot ? .on : .off
        
        ciaTimerB.intValue = Int32(info.timerB.count)
        ciaLatchB.intValue = Int32(info.timerB.latch)
        ciaRunningB.state = info.timerB.running ? .on : .off
        ciaToggleB.state = info.timerB.toggle ? .on : .off
        ciaPBoutB.state = info.timerB.pbout ? .on : .off
        ciaOneShotB.state = info.timerB.oneShot ? .on : .off
        
        todHours.intValue = Int32(info.tod.time.hours)
        todMinutes.intValue = Int32(info.tod.time.minutes)
        todSeconds.intValue = Int32(info.tod.time.seconds)
        todTenth.intValue = Int32(info.tod.time.tenth)
        todIntEnable.state = info.todIntEnable ? .on : .off
        alarmHours.intValue = Int32(info.tod.alarm.hours)
        alarmMinutes.intValue = Int32(info.tod.alarm.minutes)
        alarmSeconds.intValue = Int32(info.tod.alarm.seconds)
        alarmTenth.intValue = Int32(info.tod.alarm.tenth)
        
        ciaIcr.intValue = Int32(info.icr)
        ciaIcrBinary.intValue = Int32(info.icr)
        ciaImr.intValue = Int32(info.imr)
        ciaImrBinary.intValue = Int32(info.imr)
        ciaIntLineLow.state = info.intLine ? .off : .on
    }

}


