
(setq volsung-highlights
      '((";.*" . font-lock-comment-face)
        ("&.*?\s" . font-lock-function-name-face)
        ("\\(<>\\|>>\\|x>\\|=>\\|->\\)" . font-lock-keyword-face)       
        ("\\(\\w\\|_\\)+~" . font-lock-type-face)
        ("\\([0-9]+\\(\\.[0-9]+\\)?\\|true\\|false\\)" . font-lock-constant-face)
        ("\".*\"" . font-lock-string-face)
        ))

(define-derived-mode volsung-mode fundamental-mode "Volsung"
  "It makes sound and does things to it"
  (setq font-lock-defaults '(volsung-highlights))
  (setq font-lock-keywords-only t))

(add-to-list 'auto-mode-alist '("\\.vlsng\\'" . volsung-mode))

(provide 'volsung)
