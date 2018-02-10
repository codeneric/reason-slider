type t = {. "mobile": Js.Nullable.t(Js.boolean), "tablet": Js.Nullable.t(Js.boolean)};

[@bs.module] external bowser : t = "bowser";