// Efeitos de entrada e saida.
var animacoes = {
    "entrada": [
        "bounce", "pulse", "rubberBand", "shake", "swing", "tada", "wobble",
        "bounceIn", "bounceInDown", "bounceInLeft", "bounceInRight", "bounceInUp",
        "fadeIn", "fadeInDown", "fadeInDownBig", "fadeInLeft", "fadeInLeftBig", "fadeInRight", "fadeInRightBig", "fadeInUp", "fadeInUpBig",
        "flip", "flipInX", "flipInY",
        "lightSpeedIn",
        "rotateIn", "rotateInDownLeft", "rotateInDownRight", "rotateInDownRight", "rotateInUpLeft", "rotateInUpRight",
        "slideInDown", "slideInLeft", "slideInRight",
        "rollIn"
    ],
    "saida": [
        "bounceOut", "bounceOutDown", "bounceOutLeft", "bounceOutRight", "bounceOutUp",
        "fadeOut", "fadeOutDown", "fadeOutDownBig", "fadeOutLeft", "fadeOutLeftBig", "fadeOutRight", "fadeOutRightBig", "fadeOutUp", "fadeOutUpBig",
        "flipOutX", "flipOutY",
        "lightSpeedOut",
        "rotateOut", "rotateOutDownLeft", "rotateOutDownRight", "rotateOutUpLeft", "rotateOutUpRight",
        "slideOutLeft", "slideOutRight", "slideOutUp",
        "hinge",
        "rollOut"
    ]
};

var animacaoAnterior = "none";
var animacaoAtual = embaralhaArray(animacoes['entrada'])[0];
var tempoPadrao = 1000;
animacaoAnterior = animacaoAtual;

// Embaralha um array
function embaralhaArray(array) {
    var currentIndex = array.length, temporaryValue, randomIndex;

    // While there remain elements to shuffle...
    while (0 !== currentIndex) {

        // Pick a remaining element...
        randomIndex = Math.floor(Math.random() * currentIndex);
        currentIndex -= 1;

        // And swap it with the current element.
        temporaryValue = array[currentIndex];
        array[currentIndex] = array[randomIndex];
        array[randomIndex] = temporaryValue;
    }

    return array;
}

// Adiciona a Animacao
function adicionaAnimacao(elem, fx) {
    $(elem).addClass('animated').addClass(fx);
}

// Removo a Animacao
function removeAnimacao(elem, fx, time) {
    window.setTimeout(function () {
        $(elem).removeClass('animated').removeClass(fx);
    }, time);
}

$(document).ready(function () {
    animacaoAtual = "bounceInDown";
    adicionaAnimacao(".wrapper", animacaoAtual);
    removeAnimacao(".wrapper", animacaoAtual, tempoPadrao);
});