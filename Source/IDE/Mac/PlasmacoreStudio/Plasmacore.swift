import Cocoa

class Plasmacore
{
  static let singleton = Plasmacore();

  private init()
  {
  }

  func configure()->Plasmacore
  {
    RogueInterface_set_arg_count( Int32(Process.arguments.count) )
    for (index,arg) in Process.arguments.enumerate()
    {
      RogueInterface_set_arg_value( Int32(index), arg )
    }

    RogueInterface_configure();
    return self
  }

  func launch()->Plasmacore
  {
    RogueInterface_launch();
    return self
  }
}

