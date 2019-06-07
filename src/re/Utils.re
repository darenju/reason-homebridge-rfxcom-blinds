type order = {
  id: string,
  time: string,
  direction: string,
  duration: option(int),
};

type blind = {
  name: string,
  deviceID: string,
  openCloseSeconds: int,
  orders: list(order),
};

let decodeOrder = json =>
  Json.Decode.{
    id: json |> field("id", string),
    time: json |> field("time", string),
    direction: json |> field("direction", string),
    duration: json |> optional(field("duration", int)),
  };

let decodeBlind = json =>
  Json.Decode.{
    name: json |> field("name", string),
    deviceID: json |> field("deviceID", string),
    openCloseSeconds: json |> field("openCloseSeconds", int),
    orders: json |> field("orders", list(decodeOrder)),
  };

let decodeResponse = json =>
  Json.Decode.(list(decodeBlind, json));

let useNodeFromRef = (domRef, cb) => {
  domRef->React.Ref.current->Js.Nullable.toOption->Belt.Option.map(myRef => {
    cb(ReactDOMRe.domElementToObj(myRef));
  }) |> ignore;
};

let orderHasChanged = (one, two) =>
  one.time != two.time || one.direction != two.direction || one.duration != two.duration;

[@bs.val]
external add_keyboard_event_listener :
  (string, ReactEvent.Keyboard.t => unit) => unit =
  "addEventListener";

[@bs.val]
external remove_keyboard_event_listener :
  (string, ReactEvent.Keyboard.t => unit) => unit =
  "removeEventListener";
