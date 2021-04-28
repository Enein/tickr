<!DOCTYPE html>
<html lang="ru-RU">
<head>
	<meta charset="UTF-8">
	<meta name="viewport" content="width=device-width, initial-scale=1">
	<title>tickr.home</title>
	<script src="/core/jquery.min.js"></script>
</head>

<body>

<style>
a, input, button { font-size: 1.3em; }
body { min-height: auto; background-color: #e2e2e2; }

* {
	box-sizing: inherit;
	transition-duration: 0.2s;
	transition-property: none;
	transition-timing-function: cubic-bezier(0.42, 0, 0.58, 1);
}
label.field {
	border-radius: 0 0 2px 2px;
	color: #666;
	display: block;
	margin: 0 15px;
	padding: 10px;
	opacity: 0;
	position: relative;
	transition-property: opacity;
	z-index: 1;
	background-color: rgba(147, 192, 197, 0.11);
	border: solid rgba(116, 154, 159, 0.5);
	border-width: 0 1px 1px 1px;
	transition: .15s;
}
label.field.actived {
	background-color: rgba(0, 188, 212, 0.11);
	border: solid rgba(0, 133, 150, 0.5);
	border-width: 0 1px 1px 1px;
}
label.field > span {
	color: inherit;
	display: block;
	font-size: 22px;
	height: 20px;
	line-height: 20px;
	left: 9px;
	pointer-events: none;
	position: absolute;
	top: 28px;
	transform: scale(1) translateY(0);
	transition-property: color, font-size, top;
	z-index: 1;
}
label.field span.required::after {
	color: inherit;
	content: "*";
	display: block;
	height: 20px;
	left: -20px;
	line-height: 20px;
	position: absolute;
	text-align: center;
	top: 0;
	width: 20px;
}
label.field .psuedo_select {
	background: rgba(255, 255, 255, 0);
	position: relative;
	border-color: #666;
	border-style: solid;
	border-width: 0 0 2px 0;
	color: #666;
	cursor: pointer;
	font-size: 20px;
	height: 24px;
	line-height: 24px;
	margin: 24px 0 0 0;
	min-width: 250px;
	padding-top: 32px;
	outline: 0;
	z-index: 1;
}
label.field.actived .psuedo_select { border-color: #02877b; }
label.field .psuedo_select::after {
	background: url("data:image/svg+xml;utf8,<svg fill='#666' height='24' viewBox='0 0 24 24' width='24' xmlns='http://www.w3.org/2000/svg'> <path d='M7.41 7.84L12 12.42l4.59-4.58L18 9.25l-6 6-6-6z'/> <path d='M0-.75h24v24H0z' fill='none'/> </svg>"), no-repeat;
	content: "";
	height: 24px;
	width: 24px;
	position: absolute;
	top: 0;
	right: 0;
	transition-property: background;
}
label.field .psuedo_select .selected {
	height: 32px;
	left: 1px;
	line-height: 32px;
	opacity: 0;
	position: absolute;
	top: 0;
	transform: translateY(24px);
	transition-property: opacity, transform;
	will-change: transform;
	white-space: nowrap;
	color: #e91e63;
	max-width: 100%;
	overflow: hidden;
	background-color: #fff;
	padding: 0 5px;
	pointer-events: none;
}
label.field .psuedo_select .selected > div, label.field .psuedo_select .selected > span { display: none; }
label.field .psuedo_select ul {
	background: #fff;
	box-shadow: 0 1px 4px 0 rgba(0, 0, 0, 0.14);
	display: block;
	height: 0;
	list-style: none;
	margin-top: 2px;
	opacity: 0;
	overflow: hidden;
	padding: 0 1px;
	pointer-events: none;
	transition-property: height, opacity;
	width: 100%;
	z-index: 2;
}
label.field .psuedo_select ul li {
	padding: 18px 4px 7px 4px;
	border-bottom: 1px dashed #ddd;
	position: relative;
	color: #152d5f;
}
label.field .psuedo_select ul li > div { font-size: .6em; color: #009688; position: absolute; top: -1px; pointer-events: none; }
label.field .psuedo_select ul li > span { font-size: .7em; color: #00b02c; pointer-events: none; }
label.field .psuedo_select ul li:hover { background-color: #e2e2e2; }
label.field .deselect {
	height: 100vh;
	left: 0;
	position: fixed;
	top: 0;
	width: 100vw;
	z-index: -1;
}
label.field.focused { color: #007bed; }
label.field.focused .psuedo_select { border-color: #007bed; }
label.field.focused .psuedo_select::after {
	background: url("data:image/svg+xml;utf8,<svg fill='#007BED' height='24' viewBox='0 0 24 24' width='24' xmlns='http://www.w3.org/2000/svg'> <path d='M7.41 7.84L12 12.42l4.59-4.58L18 9.25l-6 6-6-6z'/> <path d='M0-.75h24v24H0z' fill='none'/> </svg>"), no-repeat;
}
label.field.focused .psuedo_select ul {
	opacity: 1;
	pointer-events: all;
}

div.main {
	align-items: center;
	box-sizing: border-box;
	color: #666;
	cursor: default;
	display: flex;
	font-family: sans-serif;
	height: 100%;
	justify-content: center;
	margin: 0;
	width: 100%;
}
.main form { width: 100%; box-sizing: border-box; }
.main form input { display: none; }

.opt {
	display: block;
	width: 100%;
	background-color: rgba(174, 191, 193, 0.32);
	border: 3px solid #8e8e8e;
	text-align: center;
	text-shadow: 0 0 1px #aaa;
	color: #8e8e8e;
	font-size: 1.4em;
	text-transform: uppercase;
	padding: 0.5em;
	cursor: pointer;
	letter-spacing: 1px;
	margin-top: 10px;
	transition: 0.15s;
}
.opt.chkd { color: #008596; border-color: #008596; background-color: rgba(0, 188, 212, 0.32); text-shadow: 0 0 1px #3bdff5; font-size: 1.5em; }

.next { width: 100%; background-color: #00a596; height: 10vh; cursor: pointer; font-size: 1.7em; color: #fff; text-shadow: 0 0 2px #555; border-width: 0; outline: none; text-transform: uppercase; }
.next:hover { background-color: #00b6a5; }
.next:active { background-color: #02877b; }
.subm { margin-top: 30px; }
</style>

<div class="main">

<form action="/race">
	<div>
		<input type="radio" id="newrace" name="do" value="new" checked>
		<label class="opt chkd" for="newrace">Новое событие</label>
		<input type="radio" id="exrace" name="do" value="ex">
		<label class="opt" for="exrace">Из существующих</label>
		<input type="hidden" id="raceid" name="raceid" value="0">
	</div>
	<label id="img_category_label" class="field" for="img_category" data-value="">
		<span>Событие</span>
		<div id="img_category" class="psuedo_select" name="img_category">
			<span class="selected"></span>
			<ul id="img_category_options" class="options"></ul>
		</div>
	</label>
	<div class="subm">
		<button class="next" type="submit">Далее</button>
	</div>
</form>

</div>

<script type="text/javascript">

var excnt = $EXCNT$,
	exlist = $EXLIST$;

window.onload = function () {

	var util = {
		f: {
			addStyle: function (elem, prop, val, vendors) {
				var i, ii, property, value
				if (!util.f.isElem(elem)) {
					elem = document.getElementById(elem)
				}
				if (!util.f.isArray(prop)) {
					prop = [prop]
					val = [val]
				}
				for (i = 0; i < prop.length; i += 1) {
					var thisProp = String(prop[i]),
						thisVal = String(val[i])
					if (typeof vendors !== "undefined") {
						if (!util.f.isArray(vendors)) {
							vendors.toLowerCase() == "all" ? vendors = ["webkit", "moz", "ms", "o"] : vendors = [vendors]
						}
						for (ii = 0; ii < vendors.length; ii += 1) {
							elem.style[vendors[i] + thisProp] = thisVal
						}
					}
					thisProp = thisProp.charAt(0).toLowerCase() + thisProp.slice(1)
					elem.style[thisProp] = thisVal
				}
			},
			cssLoaded: function (event) {
				var child = util.f.getTrg(event)
				child.setAttribute("media", "all")
			},
			events: {
				cancel: function (event) {
					util.f.events.prevent(event)
					util.f.events.stop(event)
				},
				prevent: function (event) {
					event = event || window.event
					event.preventDefault()
				},
				stop: function (event) {
					event = event || window.event
					event.stopPropagation()
				}
			},
			getSize: function (elem, prop) {
				return parseInt(elem.getBoundingClientRect()[prop], 10)
			},
			getTrg: function (event) {
				event = event || window.event
				if (event.srcElement) {
					return event.srcElement
				} else {
					return event.target
				}
			},
			isElem: function (elem) {
				return (util.f.isNode(elem) && elem.nodeType == 1)
			},
			isArray: function(v) {
				return (v.constructor === Array)
			},
			isNode: function(elem) {
				return (typeof Node === "object" ? elem instanceof Node : elem && typeof elem === "object" && typeof elem.nodeType === "number" && typeof elem.nodeName==="string" && elem.nodeType !== 3)
			},
			isObj: function (v) {
				return (typeof v == "object")
			},
			replaceAt: function(str, index, char) {
				return str.substr(0, index) + char + str.substr(index + char.length);
			}
		}
	},
	form = {
	f: {
		init: {
			register: function () {
				var child, children = document.getElementsByClassName("field"), i
				for (i = 0; i < children.length; i += 1) {
					child = children[i]
					util.f.addStyle(child, "Opacity", 1)
				}
				children = document.getElementsByClassName("psuedo_select")
				for (i = 0; i < children.length; i += 1) {
					child = children[i]
					child.addEventListener("click", form.f.select.toggle)
				}
			},
			unregister: function () {
				//just here as a formallity
				//call this to stop all ongoing timeouts are ready the page for some sort of json re-route
			}
		},
		select: {
			blur: function (field) {
				field.classList.remove("focused")
				var child, children = field.childNodes, i, ii, nested_child, nested_children
				for (i = 0; i < children.length; i += 1) {
					child = children[i]
					if (util.f.isElem(child)) {
						if (child.classList.contains("deselect")) {
							child.parentNode.removeChild(child)
						} else if (child.tagName == "SPAN") {
							if (!field.dataset.value) {
								util.f.addStyle(child, ["FontSize", "Top"], ["22px", "32px"])
							}
						} else if (child.classList.contains("psuedo_select")) {
							nested_children = child.childNodes
							for (ii = 0; ii < nested_children.length; ii += 1) {
								nested_child = nested_children[ii]
								if (util.f.isElem(nested_child)) {
									if (nested_child.tagName == "SPAN") {
										if (!field.dataset.value) {
											util.f.addStyle(nested_child, ["Opacity", "Transform"], [0, "translateY(24px)"])
										}
									} else if (nested_child.tagName == "UL") {
											util.f.addStyle(nested_child, ["Height", "Opacity"], [0, 0])
									}
								}
							}
						}
					}
				}
			},
			focus: function (field) {
				field.classList.add("focused")
				var bool = false, child, children = field.childNodes, i, ii, iii, nested_child, nested_children, nested_nested_child, nested_nested_children, size = 0
				for (i = 0; i < children.length; i += 1) {
					child = children[i]
					util.f.isElem(child) && child.classList.contains("deselect") ? bool = true : null
				}
				if (!bool) {
					child = document.createElement("div")
					child.className = "deselect"
					child.addEventListener("click", form.f.select.toggle)
					field.insertBefore(child, children[0])
				}
				for (i = 0; i < children.length; i += 1) {
					child = children[i]
					if (util.f.isElem(child) && child.classList.contains("psuedo_select")) {
						nested_children = child.childNodes
						for (ii = 0; ii < nested_children.length; ii += 1) {
							nested_child = nested_children[ii]
							if (util.f.isElem(nested_child) && nested_child.tagName == "UL") {
								size = 0
								nested_nested_children = nested_child.childNodes
								for (iii = 0; iii < nested_nested_children.length; iii += 1) {
									nested_nested_child = nested_nested_children[iii]
									if (util.f.isElem(nested_nested_child) && nested_nested_child.tagName == "LI") {
										size += util.f.getSize(nested_nested_child, "height")
									}
								}
								util.f.addStyle(nested_child, ["Height", "Opacity"], [size + "px", 1])
							}
						}
					}
				}
			},
			selection: function (child, parent) {
				var children = parent.childNodes, i, ii, nested_child, nested_children, time = 0, value
				if (util.f.isElem(child) && util.f.isElem(parent)) {
					parent.dataset.value = child.dataset.value
					value = child.innerHTML
				}
				for (i = 0; i < children.length; i += 1) {
					child = children[i]
					if (util.f.isElem(child)) {
						if (child.classList.contains("psuedo_select")) {
							nested_children = child.childNodes
							for (ii = 0; ii < nested_children.length; ii += 1) {
								nested_child = nested_children[ii]
								if (util.f.isElem(nested_child) && nested_child.classList.contains("selected")) {
									if (nested_child.innerHTML)  {
										time = 1E2
										util.f.addStyle(nested_child, ["Opacity", "Transform"], [0, "translateY(24px)"], "all")
									}
									setTimeout(function (c, v) {
										c.innerHTML = v
										util.f.addStyle(c, ["Opacity", "Transform", "TransitionDuration"], [1, "translateY(0px)", ".1s"], "all")
									}, time, nested_child, value)
								}
							}
						} else if (child.tagName == "SPAN") {
							util.f.addStyle(child, ["FontSize", "Top"], ["16px", "8px"])
					   }
				   }
				}
				$('#raceid').val( document.getElementById('img_category_label').dataset.value );
				$('#exrace').click();
			},
			toggle: function (event) {
				util.f.events.stop(event)
				var child = util.f.getTrg(event), children, i, parent
				console.log(child.classList);
				switch (true) {
					case (child.classList.contains("psuedo_select")):
					case (child.classList.contains("deselect")):
						parent = child.parentNode
						break
					case (child.classList.contains("options")):
						parent = child.parentNode.parentNode
						break
					case (child.classList.contains("option")):
						parent = child.parentNode.parentNode.parentNode
						form.f.select.selection(child, parent)
						break
				}
				parent.classList.contains("focused") ? form.f.select.blur(parent) : form.f.select.focus(parent)
			}
		}
	}}
	form.f.init.register();

	if (excnt < 1)
		$('#exrace').prop('disabled', 'disabled');
	else {
		exlist.forEach( function(el) {
			var date = el.created.split('T')[0],
				prtcl = el.protocol.split('/').pop();
			var prtcl_str = '';
			if (prtcl.length > 0)
				prtcl_str = ' <span>(' + prtcl + ')</span>';
			$('#img_category_options').append('<li class="option" data-value="' + el.id + '"><div>' + date + '</div> ' + el.name + prtcl_str + '</li>');
		});
	}

	$('input:radio[name="do"]').click(function(){
		if ($(this).val() == 'ex') {
			$('#img_category_label').addClass('actived');
		} else {
			$('#img_category_label').removeClass('actived');
		}
		$(".opt").each(function () { $(this).removeClass('chkd'); } );
		$("label[for='" + $(this).attr('id') + "']").addClass('chkd');
	});

}

</script>

</body>

</html>