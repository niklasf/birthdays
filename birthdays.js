(function ($) {

Drupal.behaviors.birthdays = {
  attach: function (context) {
    $('.birthdays-datepicker', context).datepicker({
      dateFormat: 'yy/mm/dd',
      autoSize: true,
      changeMonth: true,
      changeYear: true,
      maxDate: '+0',
      minDate: '-115y',
      yearRange: '-115:+0'
    });
  }
};

})(jQuery);
