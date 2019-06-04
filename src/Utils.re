type schedule = {
  time: string,
  stopAfter: int,
};

type blind = {
  name: string,
  openCloseSeconds: int,
  openSchedule: schedule,
  closeSchedule: schedule,
};

type blinds = {
  success: bool,
  data: array(blind),
};

type update = {
  name: string,
  schedule,
  part: string,
};

type updateReponse = {
  success: bool,
  message: string,
};

let decodeSchedule = json =>
  Json.Decode.{
    time: json |> field("time", string),
    stopAfter: json |> field("stopAfter", int),
  };

let decodeBlind = json: blind =>
  Json.Decode.{
    name: json |> field("name", string),
    openCloseSeconds: json |> field("openCloseSeconds", int),
    openSchedule: json |> field("openSchedule", decodeSchedule),
    closeSchedule: json |> field("closeSchedule", decodeSchedule),
  };

let decodeBlinds = json: blinds =>
  Json.Decode.{
    success: json |> field("success", bool),
    data: json |> field("data", array(decodeBlind)),
  };

let decodeUpdateResponse = json: updateReponse =>
  Json.Decode.{
    success: json |> field("success", bool),
    message: json |> field("message", string),
  };

let encodeSchedule = schedule => (
  schedule.time ++
  switch (schedule.stopAfter) {
  | 0 => ""
  | stopAfter => "/" ++ string_of_int(stopAfter)
  }
);
