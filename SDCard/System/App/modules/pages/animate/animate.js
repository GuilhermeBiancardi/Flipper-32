$(document).ready(function () {
    $('.animation_select').click(function () {
        $('#animation_box').removeAttr('class').attr('class', '');
        var animation = $(this).attr("data-animation");
        $('#animation_box').addClass('animated');
        $('#animation_box').addClass(animation);
        return false;
    });
});