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

		$(".form-div input.uid").val($("#user").find(".id").text());

		// Populating edit event form on event click
		$(".event").click(function() {
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

		// Ajax form submits
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
		    e.unbind();
		});

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

		
		


















	});



})();