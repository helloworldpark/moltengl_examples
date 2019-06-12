//
//  DummyOperation.swift
//  DrawLoadDemo-iOS
//
//  Created by Sean Park on 12/06/2019.
//

import Foundation


@objc class DummyClass: NSObject {
    
    var tag: String
    
    init(tag: String) {
        self.tag = tag
    }
    
    public func printTag(msg: String) {
        print("[\(self.tag)] \(msg)")
    }
}
