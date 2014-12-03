(function() {

	$(document).ready(function() {


		// Register new account
		$("#container .register-link").click(function() {
			$(this).next().toggle();
		});

		// Groups
		$("#user .groups").click(function() {
			$("#groups").toggle();
		});

		// Put user id into event form
		$(".form-div input.uid").val($("#user").find(".id").text());

		// Populating edit event form on event click
		$(".event").click(function() {
			// Delegate active class appropriately
			$("#table-div .event").removeClass("active");
			$(this).addClass("active");

			$("#edit-event .modified").hide();

			$("#edit-event input[name='name']").val($(this).find(".name").text());
			var month = $("#month").attr("data-value");
			var day = $(this).parent().find(".day-num").text();
			var time = $(this).find(".time").text();
			var hour = time.substring(0, time.indexOf(":"));

			$("#edit-event input[name='datetime']")
				.val($("#year").text() + "-" + (month < 10 ? "0" + month : month)
					+ "-" + (day < 10 ? "0" + day : day)
					+ " " + (hour < 10 ? "0" + time : time));


			$("#edit-event input.event-id").val($(this).attr("data-value"));
			$("#edit-event").show();
		});

		//// Ajax form submits

		// Edit event
		$("#edit-event form").submit(function(e) {
			var postData = $(this).serializeArray();
		    var formURL = $(this).attr("action");
		    $.ajax({
		        url : formURL,
		        type: "POST",
		        data : postData,
		        success: function(data, textStatus, jqXHR) {
		            //data: data returned from server
		        },
		        error: function(jqXHR, textStatus, errorThrown) {
		            //if fails      
		        }
		    });
		    e.preventDefault();
		    e.unbind();});

		// Create event
		$("#create-event form").submit(function(e) {
			var postData = $(this).serializeArray();
		    var formURL = $(this).attr("action");
		    $.ajax({
		        url : formURL,
		        type: "POST",
		        data : postData,
		        success: function(data, textStatus, jqXHR) {
		            //data: data returned from server
		        },
		        error: function(jqXHR, textStatus, errorThrown) {
		            //if fails      
		        }
		    });
		    e.preventDefault();
		    e.unbind();
		});

		// Create group
		$("#groups .make-group form").submit(function(e) {
			var postData = $(this).serializeArray();
		    var formURL = $(this).attr("action");
		    $.ajax({
		        url : formURL,
		        type: "POST",
		        data : postData,
		        success: function(data, textStatus, jqXHR) {
		            //data: data returned from server
		        },
		        error: function(jqXHR, textStatus, errorThrown) {
		            //if fails      
		        }
		    });
		    e.preventDefault();
		    e.unbind();
		});

		// Add to group
		$("#groups .add-to-group form").submit(function(e) {
			var postData = $(this).serializeArray();
		    var formURL = $(this).attr("action");
		    $.ajax({
		        url : formURL,
		        type: "POST",
		        data : postData,
		        success: function(data, textStatus, jqXHR) {
		            //data: data returned from server
		        },
		        error: function(jqXHR, textStatus, errorThrown) {
		            //if fails      
		        }
		    });
		    e.preventDefault();
		    e.unbind();
		});

		// Query getEvents for the current user
		(function poll(){
		   setTimeout(function(){
		      $.ajax({ url: "getEventsServer", success: function(data){
		        updateEvents(JSON.parse(data));

		        // Call this function (in 10 seconds)
		        poll();
		      }, dataType: "json"});
		  }, 10000);
		})();



	});

	function updateEvents(data) {
		// draws events that fall within current month on calendar
		//	(only if they are newer than the current event's timestamp?)
		// data is JSON object:
			// [
			// 	{
			// 		"id": "1"
			// 		"name": "event1",
			// 		"year": "2014",
			// 		"month": "3",
			// 		"day": "25",
			// 		"time": "3:45"
			// 	},
			// 	{
			// 		"id": "4"
			// 		"name": "event4",
			// 		"year": "2014",
			// 		"month": "6",
			// 		"day": "2",
			// 		"time": "9:30"
			// 	},
			// ]
			//		Should we add a timestamp field to event? then we can check that too

		var curYear = parseInt($("#year").text());
		var curMonth = parseInt($("#month").attr("data-value"));

		// clear out inactive events
		$("#table-div td.real-day .event:not(.active)").remove();
		var active = $("#table-div td.real-day .event.active");

		for (var event in data) {
			if (event.year == curYear && event.month == curMonth) {

				if (event.id == active.attr("data-value")
					&& (active.children(".time").text() != event.time
						|| active.children(".name").text() != event.name
						|| active.siblings("day-num").text() != event.day)) {
					// An update has been made to the currently active event
					$("#edit-event .modified").show();
				} else if (event.id != activeId) {
					// in the right month and not active, so append the event to the right day

					var td = $("#table-div td.real-day.day-" + event.day);

					var newEvent = $("<div class='event'></div>");
					newEvent.attr("data-value", event.id);
					newEvent.append("<span class='time'>" + event.time + "</span>");
					newEvent.append(": ");
					newEvent.append("<span class='name'>" + event.name + "</span>");

					td.append(newEvent);
				}
			}
		}

		$("#table-div td.real-day .event.active").each(function() {

		});


	}


})();