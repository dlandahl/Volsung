
(setq volsung-highlights
    '((";.*" . font-lock-comment-face)
    ("&.*?\s" . font-lock-function-name-face)
    ("\\(<>\\|>>\\|x>\\|=>\\|->\\)" . font-lock-keyword-face)		
    ("\\(\\w\\|_\\)+~" . font-lock-type-face)
    ("[0-9]+\\(\\.[0-9]+\\)?" . font-lock-constant-face)
		))

(define-derived-mode volsung-mode fundamental-mode "vlsng"
    "It makes sound and does things to it"
    (setq font-lock-defaults '(volsung-highlights))
    (defvar font-lock-keywords-only nil))

(add-to-list 'auto-mode-alist '("\\.vlsng\\'" . volsung-mode))

(provide 'volsung)

