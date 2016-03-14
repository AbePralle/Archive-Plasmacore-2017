import Cocoa

class Plasmacore
{
  static let singleton = Plasmacore();

  func sayHello()
  {
    NSLog( "Hello!\n" );
  }

  class func message( type:String )->PlasmacoreMessage
  {
    return PlasmacoreMessage( type:type );
  }
}

