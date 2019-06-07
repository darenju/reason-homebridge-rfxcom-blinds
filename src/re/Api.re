open Utils;

type updateResponse = {
  message: string,
};
type addResponse = {
  newId: string,
  message: string,
};

[@bs.val] external baseUrl: string = "baseUrl";

let decodeUpdateResponse = json =>
  Json.Decode.{
    message: json |> field("message", string),
  };

let decodeAddResponse = json =>
  Json.Decode.{
    newId: json |> field("id", string),
    message: json |> field("message", string),
  };

let fetchBlinds = (callback) => {
  Js.Promise.(
      Fetch.fetch(baseUrl ++ "blinds")
      |> then_(Fetch.Response.json)
      |> then_(json =>
        Utils.decodeResponse(json)
        |> (blinds => callback(blinds))
        |> resolve)
    ) |> ignore;
};

let update = (order, callback) => {
  let { time, direction, duration } = order;
  let url = baseUrl ++ "update/" ++ order.id ++
    "?time=" ++ time ++
    "&direction=" ++ direction ++
    switch (duration) {
      | None => ""
      | Some(v) => ("&duration=" ++ string_of_int(v))
    };

  Js.Promise.(
    Fetch.fetch(url)
    |> then_(Fetch.Response.json)
    |> then_(json =>
      decodeUpdateResponse(json)
      |> callback
      |> resolve)
  ) |> ignore;
};

let add = (blindName, order: order, callback) => {
  let { direction, time, duration } = order;
  let url = baseUrl ++ "add/" ++
    Js.Global.encodeURIComponent(blindName) ++
    "?direction=" ++ direction ++
    "&time=" ++ time ++
    switch (duration) {
      | None => ""
      | Some(v) => ("&duration=" ++ string_of_int(v))
    };

  Js.Promise.(
    Fetch.fetch(url)
    |> then_(Fetch.Response.json)
    |> then_(json =>
      decodeAddResponse(json)
      |> callback
      |> resolve)
  ) |> ignore;
};

let delete = (order, callback) => {
  let url = baseUrl ++ "delete/" ++ order.id;

  Js.Promise.(
    Fetch.fetch(url)
    |> then_(Fetch.Response.json)
    |> then_(json =>
      decodeUpdateResponse(json)
      |> callback
      |> resolve)
  ) |> ignore;
};
