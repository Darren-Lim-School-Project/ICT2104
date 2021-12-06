<html>
  <head>
    <title>IRAcer Controller</title>
  </head>
  
  <body>
    <button id="l" class="direction">Arrow Left</button>
    <button id="r" class="direction">Arrow Right</button>
    <button id="f" class="direction">Arrow Up</button>
    <button id="b" class="direction">Arrow Down</button>
    
  <script src="jquery.min.js"></script>
  <script type="text/javascript">
    $(document).ready(function() {
      $(".direction").click(function() {
        var d = $(this).attr('id');
        $.get("http://192.168.1.142:8000/", {pin:d});
      });
    });
  </script>
  </body>
</html>
