(function($) {

/**
 * Give live feedback on dateformats.
 */
Drupal.behaviors.birthdays = {
  attach: function (context, settings) {
    for (var value in settings.birthdays) {
      (function(settings) {
        var source = '#edit-' + value;
        var suffix = source + '-suffix';

        // Attach keyup handler to custom format inputs.
        $('input' + source, context).once('birthdays').keyup(function () {
          var input = $(this);
          var url = settings.lookup + (settings.lookup.match(/\?q=/) ? '&format=' : '?format=') + encodeURIComponent(input.val());
          $.getJSON(url, function (data) {
            $(suffix).empty().append(' ' + settings.text + ': <em>' + data + '</em>');
          });
        });
      })(settings.birthdays[value]);
    }
  }
};

})(jQuery);
