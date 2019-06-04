open Utils;

let getValue = (e: ReactEvent.Form.t) =>
  ReactEvent.Form.target(e)##value;

type scheduleTexts = {
  name: string,
  schedule: string,
  timeLabel: string,
};

let renderSchedule = (
  ~name: string,
  ~texts: scheduleTexts,
  ~onSetTime: string => unit,
  ~onSetStopAfter: int => unit,
  ~schedule: schedule,
  ~openCloseSeconds: int,
  ~disabled: bool,
) => {
  let { time, stopAfter } = schedule;

  <fieldset>
    <legend>{texts.name |> ReasonReact.string} </legend>
    <label htmlFor={texts.schedule ++ "Time-" ++ name}>
      <em className=("icon icon-blind-" ++ texts.schedule) />

      {texts.timeLabel |> ReasonReact.string}
    </label>
    <input
      id={texts.schedule ++ "Time-" ++ name}
      type_="time"
      value={time}
      onChange={e => e |> getValue |> onSetTime}
      disabled
    />
    <label htmlFor={"toggle" ++ texts.schedule ++ "StopAfter-" ++ name}>
      {{js|S’arrêter au bout d’un certain temps|js} |> ReasonReact.string}
      <input
        id={"toggle" ++ texts.schedule ++ "StopAfter-" ++ name}
        type_="checkbox"
        checked={stopAfter > 0}
        onChange={_ => onSetStopAfter(stopAfter > 0 ? 0 : openCloseSeconds / 2)}
        disabled
      />
    </label>
    {
      switch (stopAfter) {
      | 0 => React.null
      | value =>
        <>
          <input
            type_="number"
            min=1
            max={string_of_int(openCloseSeconds - 1)}
            value={string_of_int(value)}
            onChange={e => e |> getValue |> int_of_string |> onSetStopAfter}
            disabled
          />
          {{js|seconde•s|js} |> ReasonReact.string}
        </>
      }
    }
  </fieldset>
};

[@react.component]
let make = (
  ~blind: blind,
  ~index: int,
  ~disabled: bool,
  ~onUpdate: (int, blind, update) => unit,
) => {
  let {
    name,
    openCloseSeconds,
    openSchedule,
    closeSchedule,
  } = blind;

  let onSetOpenTime = time => {
    let newSchedule = { ...openSchedule, time };
    let update = { name, schedule: newSchedule, part: "open" };

    onUpdate(index, { ...blind, openSchedule: newSchedule }, update);
  };
  let onSetCloseTime = time => {
    let newSchedule = { ...closeSchedule, time };
    let update = { name, schedule: newSchedule, part: "close" };

    onUpdate(index, { ...blind, closeSchedule: newSchedule }, update);
  };
  let onSetOpenStopAfter = stopAfter => {
    let newSchedule = { ...openSchedule, stopAfter };
    let update = { name, schedule: newSchedule, part: "open" };

    onUpdate(index, { ...blind, openSchedule: newSchedule }, update);
  };
  let onSetCloseStopAfter = stopAfter => {
    let newSchedule = { ...closeSchedule, stopAfter };
    let update = { name, schedule: newSchedule, part: "close" };

    onUpdate(index, { ...blind, closeSchedule: newSchedule }, update);
  };

  <div>
    <h2>(name |> ReasonReact.string)</h2>

    {
      renderSchedule(
        ~name=name,
        ~texts={
          name: "Ouverture",
          timeLabel: {js|Heure d’ouverture|js},
          schedule: "open",
        },
        ~onSetTime=onSetOpenTime,
        ~onSetStopAfter=onSetOpenStopAfter,
        ~schedule=openSchedule,
        ~openCloseSeconds=openCloseSeconds,
        ~disabled=disabled
      )
    }

    {
      renderSchedule(
        ~name=name,
        ~texts={
          name: "Fermeture",
          timeLabel: "Heure de fermeture",
          schedule: "close",
        },
        ~onSetTime=onSetCloseTime,
        ~onSetStopAfter=onSetCloseStopAfter,
        ~schedule=closeSchedule,
        ~openCloseSeconds=openCloseSeconds,
        ~disabled=disabled
      )
    }
  </div>
};
