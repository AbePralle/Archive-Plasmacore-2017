//
//  LaunchWindow.swift
//  PlasmacoreStudio
//
//  Created by Abe Pralle on 4/4/16.
//  Copyright © 2016 Abe Pralle. All rights reserved.
//

import Cocoa

class LaunchWindow: NSWindowController
{
  @IBOutlet weak var b_new: NSButton!
  
  override func windowDidLoad()
  {
    super.windowDidLoad()

  }
  
  override func awakeFromNib()
  {
    let title = NSMutableAttributedString(string:"Testing")
    title.addAttribute( NSForegroundColorAttributeName, value:NSColor(red:1,green:0,blue:0,alpha:1), range:NSMakeRange(0,title.length) )
    b_new.attributedTitle = title
  
    
    //b_new.title = NSAttributedString(
  }
}
