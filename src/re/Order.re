open Utils;

type state = {
  editing: bool,
  ready: bool,
  direction: string,
};

type action =
  | SetReady(bool)
  | SetEditing(bool)
  | SetDirection(string);

let createReducer = (state, action) =>
  switch (action) {
  | SetReady(ready) => { ...state, ready }
  | SetEditing(editing) => { ...state, editing }
  | SetDirection(direction) => { ...state, direction }
  };

let mouseDownTimer = ref(None);

[@react.component]
let make = (
  ~order,
  ~openCloseSeconds,
  ~onOrderEdit,
  ~deleting,
  ~onRequestDelete,
  ~onDelete,
  ~onCancel=unit => unit,
  ~isNew=false,
) => {
  let { time, direction, duration } = order;

  let (state, dispatch) = React.useReducer(createReducer, {
    editing: false,
    ready: false,
    direction,
  });
  let { editing, ready } = state;

  let timeRef = React.useRef(Js.Nullable.null);
  let durationRef = React.useRef(Js.Nullable.null);

  let focusTimeField = () => {
    useNodeFromRef(timeRef, node => {
      node##focus();
      node##setSelectionRange(0, node##value##length);
    });
  };

  // When finish editing, send the event.
  React.useEffect1(() => {
    switch (editing, ready) {
    | (true, true) => focusTimeField()
    | (false, true) => {
        let time = ref("");
        let duration: ref(option(int)) = ref(None);

        useNodeFromRef(timeRef, node => {
          time := node##value;
          node##blur();
        });

        useNodeFromRef(durationRef, node => {
          let durationString: string = node##value;
          if (durationString|>String.length > 0) {
            duration := Some(int_of_string(durationString));
          }
          node##blur();
        });

        let newOrder = {
          id: order.id,
          time: time^,
          direction: state.direction,
          duration: duration^,
        };

        if (orderHasChanged(order, newOrder)) {
          onOrderEdit(newOrder);
        }
      }
    | _ => ()
    };

    None;
  }, [|editing|]);

  // Remove edit mode on delete.
  React.useEffect1(() => {
    if (deleting) {
      dispatch(SetEditing(false));
    }

    None;
  }, [|deleting|]);

  // On component mount, set ready…
  React.useEffect0(() => {
    dispatch(SetReady(true));

    // Register a keyDown event listener to stop editing.
    let callback = e => {
      if (ReactEvent.Keyboard.which(e) == 27) {
        if (!isNew) {
          dispatch(SetEditing(false));
        }
        onCancel();
      }
    };

    if (isNew && !deleting) {
      dispatch(SetEditing(true));
      focusTimeField();
    };

    Utils.add_keyboard_event_listener("keydown", callback);

    Some(() => {
      Utils.remove_keyboard_event_listener("keydown", callback);
    });
  });

  let onMouseDown = (e) => {
    if (!editing && !deleting) {
      ReactEvent.Mouse.preventDefault(e);
    };

    if (deleting) {
      onDelete(order);
    };

    mouseDownTimer := Some(Js.Global.setTimeout(() => {
      onRequestDelete();
    }, 500));
  };

  let onMouseUp = (e) => {
    ReactEvent.Mouse.preventDefault(e);

    if (!deleting) {
      dispatch(SetEditing(true));
    }

    switch (mouseDownTimer^) {
    | None => ()
    | Some(t) => {
        Js.Global.clearTimeout(t);
        mouseDownTimer := None;
      }
    }
  };

  let onSubmit = e => {
    ReactEvent.Form.preventDefault(e);

    useNodeFromRef(timeRef, node => node##blur());
    useNodeFromRef(durationRef, node => node##blur());

    dispatch(SetEditing(false));
  };

  let onToggleDirection = e => {
    e |> ignore;

    if (editing) {
      switch (state.direction) {
      | "up" => dispatch(SetDirection("down"))
      | _ => dispatch(SetDirection("up"))
      };
    }
  };

  // Attributes.
  let className = "order" ++
    (editing && !deleting ? " order--editing" : "") ++
    (deleting ? " order--deleting" : "");
  let buttonClassName = "order__button" ++ (editing ? " order__button--visible": "");

  let displayedDuration = switch (duration) {
    | None => ""
    | Some(value) => string_of_int(value)
    };

  let durationPlaceholder = switch (editing) {
    | true => string_of_int(openCloseSeconds / 2)
    | false => "-"
    };

  // Render.
  <form
    className
    onMouseDown=(onMouseDown)
    onMouseUp=(onMouseUp)
    onSubmit=(onSubmit)
  >
    <button
      className=(buttonClassName)
      type_="submit"
    >
      {"OK" |> ReasonReact.string}
    </button>

    <em
      className=("order__icon icon icon-blind-" ++ state.direction)
      onMouseDown=(onToggleDirection)
    />

    <span className="order__field">
      <em className="icon icon-time order__field__icon" />

      {
        <input
          ref=(ReactDOMRe.Ref.domRef(timeRef))
          type_="text"
          className="order__input"
          defaultValue=(time)
          readOnly=(!editing)
          placeholder="hh:mm"
          pattern="\\d{2}:\\d{2}"
          required=(true)
        />
      }
    </span>
    <span className="order__field">
      <em className="icon icon-duration order__field__icon" />

      {
        <input
          ref=(ReactDOMRe.Ref.domRef(durationRef))
          type_="number"
          className="order__input"
          defaultValue=(displayedDuration)
          readOnly=(!editing)
          placeholder=(durationPlaceholder)
        />
      }
    </span>
  </form>
};
