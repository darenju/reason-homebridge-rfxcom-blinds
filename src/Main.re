open Utils;

type state = {
  blinds: array(blind),
  updates: list(update),
  loading: bool,
};

type action =
  | SetBlinds(array(blind))
  | SetBlindAtIndex(int, blind)
  | QueueUpdate(update)
  | SetLoading(bool);

let createReducer = (state, action) =>
  switch (action) {
  | SetBlinds(blinds) => { ...state, blinds }
  | SetBlindAtIndex(index, blind) => {
      let blinds = state.blinds->Array.copy;
      blinds->Array.set(index, blind);

      {
        ...state,
        blinds,
      };
    }
  | QueueUpdate(update) => {
      let { updates } = state;
      let newUpdates = [
        update,
        ...updates,
      ];

      // Don't update twice for the same situation.
      switch (state.updates|>List.find(testUpdate =>
        testUpdate.name == update.name && testUpdate.part == update.part && testUpdate.schedule != update.schedule
      )) {
      | foundUpdate => {
          ...state,
          updates: newUpdates|>List.filter(testUpdate => testUpdate != foundUpdate),
        }
      | exception Not_found => {
          ...state,
          updates: newUpdates,
        }
      }
    }
  | SetLoading(loading) => { ...state, loading }
  };

[@react.component]
let make = (_) => {
  let (state, dispatch) = React.useReducer(createReducer, {
    blinds: [||],
    loading: true,
    updates: [],
  });

  React.useEffect0(() => {
    Js.Promise.(
      Fetch.fetch("http://192.168.1.12:18081/?action=getSchedule")
        |> then_(Fetch.Response.json)
        |> then_(json =>
            json |> decodeBlinds
                |> (data => {
                    dispatch(SetBlinds(data.data));
                    dispatch(SetLoading(false));
                  }
                ) |> resolve
        )
    ) |> ignore;

    None;
  });

  let { blinds, loading, updates } = state;

  let handleUpdate = React.useCallback((update: update) => {
    let { name, schedule, part } = update;
    let direction = if (part == "open") "up" else "close";
    let url = "http://192.168.1.12:18081/?action=setSchedule&direction=" ++
      direction ++
      "&remoteName=" ++ Js.Global.encodeURIComponent(name) ++
      "&schedule=" ++ schedule->encodeSchedule->Js.Global.encodeURIComponent;

    dispatch(SetLoading(true));

    Js.Promise.(
      Fetch.fetch(url)
        |> then_(Fetch.Response.json)
        |> then_(json =>
            json |> decodeUpdateResponse
                 // TODO: Add feedback.
                 |> (data => {
                      data |> ignore;
                      dispatch(SetLoading(false));
                 }) |> resolve
          )
    ) |> ignore;
  });

  let onSubmit = React.useCallback(e => {
    ReactEvent.Form.preventDefault(e);

    updates|>List.iter(handleUpdate);
  });

  let onUpdateBlind = React.useCallback((index, blind, update) => {
    dispatch(SetBlindAtIndex(index, blind));
    dispatch(QueueUpdate(update));
  });

  <form onSubmit>
    (
      ReasonReact.array(blinds |> Array.mapi((index, blind) => {
        <Blind
          key=("blind-" ++ (index |> string_of_int))
          onUpdate=(onUpdateBlind)
          blind
          index
          disabled=(loading)
        />
      }))
    )
    <button type_="submit">
      {"Valider"->ReasonReact.string}
    </button>
  </form>;
};
